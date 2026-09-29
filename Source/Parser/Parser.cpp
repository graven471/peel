#include "Parser.hpp"

#include <algorithm>
#include <ranges>
#include <vector>

#include "PETypes.hpp"

[[nodiscard]] PeelResult<PEImage> PEParser::parse()
{
  PeelResult<ImageDosHeader> dos_header = parse_dos_header(mapped_bytes.subspan(0, sizeof(ImageDosHeader)));

  if(!dos_header)
    return std::unexpected(dos_header.error());

  // [PE_SIGNATURE | FileHeader (20 bytes) | OptionalHeader]
  PeelResult<ImageFileHeader> file_header =
      parse_file_header(mapped_bytes.subspan(dos_header->LfaNew, sizeof(ImageFileHeader) + SIGNATURE.size()));

  if(!file_header)
    return std::unexpected(file_header.error());

  // every image file has an optional header header that provides info to
  // loader. this header is optional in object files, for image files this
  // header is required it's size is not fixed FileHeader::SizeOfOptionalHeader
  // must be used to validate
  const std::size_t optional_header_offset = dos_header->LfaNew + SIGNATURE.size() + sizeof(::ImageFileHeader);

  PeelResult<ImageOptionalHeader> optional_header =
      parse_optional_header(mapped_bytes.subspan(optional_header_offset, file_header->SizeOfOptionalHeader));

  if(!optional_header)
    return std::unexpected(optional_header.error());

  std::println("Machine: \t {}", MachineToStringView(file_header->Machine));

  ImageNTHeaders nt_headers{
      .FileHeader     = *file_header,
      .OptionalHeader = *optional_header,
  };

  // todo: i don't like this
  std::visit([&](auto&& header) { nt_headers.Format = header.Format; }, nt_headers.OptionalHeader);

  const std::size_t section_header_offset =
      dos_header->LfaNew + sizeof(SIGNATURE) + sizeof(ImageFileHeader) + file_header->SizeOfOptionalHeader;

  PeelResult<std::vector<ImageSection>> sections =
      parse_section_headers(mapped_bytes.subspan(section_header_offset, file_header->NumberOfSections * sizeof(ImageSectionHeader)),
                            file_header->NumberOfSections);

  if(!sections)
    return std::unexpected(sections.error());

  std::vector<Import> imports = parse_imports(*sections, nt_headers.OptionalHeader);

  return PEImage{.DosHeader = *dos_header, .NTHeaders = nt_headers, .SectionHeaders = std::move(*sections), .Imports = std::move(imports)};
}

PeelResult<ImageDosHeader> PEParser::parse_dos_header(std::span<const std::byte> dos_header)
{
  ImageDosHeader header{};

  std::memcpy(&header, dos_header.data(), sizeof(ImageDosHeader));

  if(header.Magic != PE_MAGIC)
  {
    return std::unexpected(PeelError{PeelErrorCode::PEEL_INVALID_MAGIC});
  }

  return header;
}

PeelResult<ImageFileHeader> PEParser::parse_file_header(std::span<const std::byte> image_header)
{
  if(!std::ranges::equal(SIGNATURE, image_header.subspan(0, SIGNATURE.size())))
  {
    return std::unexpected(PeelError{PeelErrorCode::PEEL_INVALID_SIGNATURE});
  }

  ImageFileHeader header{};

  // skip 4 bytes of signature
  std::memcpy(&header, image_header.data() + SIGNATURE.size(), sizeof(ImageFileHeader));

  return header;
}

PeelResult<ImageOptionalHeader> PEParser::parse_optional_header(std::span<const std::byte> optional_header)
{
  uint16_t magic{0};

  std::memcpy(&magic, optional_header.subspan(0, sizeof(uint16_t)).data(), sizeof(uint16_t));

  switch(static_cast<PEFormat>(magic))
  {
    case PEFormat::PE32: {
      ImageOptionalHeader32 header{};

      std::memcpy(&header, optional_header.data(), sizeof(ImageOptionalHeader32));

      return ImageOptionalHeader{header};
    }

    case PEFormat::PE64: {
      ImageOptionalHeader64 header{};

      std::memcpy(&header, optional_header.data(), sizeof(ImageOptionalHeader64));

      return ImageOptionalHeader{header};
    }

    default:
      return std::unexpected(PeelError{PeelErrorCode::PEEL_UNSUPPORTED_PE_VERSION});
  }
}

PeelResult<std::vector<ImageSection>> PEParser::parse_section_headers(std::span<const std::byte> in_section_header,
                                                                      std::uint32_t              section_count)
{
  std::vector<ImageSection> sections(section_count);

  // TODO: handle this in better way
  for(std::size_t index = 0; index < section_count; index++)
  {
    std::memcpy(&sections[index].Header, in_section_header.data() + index * sizeof(ImageSectionHeader), sizeof(ImageSectionHeader));

    const uint32_t raw_data_offset = sections[index].Header.PointerToRawData;
    const uint32_t raw_data_size   = sections[index].Header.SizeOfRawData;

    if(raw_data_offset > mapped_bytes.size() || raw_data_size > mapped_bytes.size() - raw_data_offset) [[unlikely]]
      return std::unexpected(PeelError{PeelErrorCode::PEEL_INVALID_SECTION});

    sections[index].Data = mapped_bytes.subspan(raw_data_offset, raw_data_size);
  }

  return sections;
}

// TODO: not a fastest or solid implementation comeback and improve it
std::vector<Import> PEParser::parse_imports(std::span<const ImageSection> sections, ImageOptionalHeader& optional_header)
{
  auto parse_import_names = [&](std::uint32_t import_lookup_table_offset, const ImageSectionHeader& section_header,
                                std::vector<std::string_view>& names) {
    for(std::size_t index = 0;; index++)
    {
      // todo: handle PE32 in that case this is uint32_t
      std::uint64_t import_lookup_entry;

      std::memcpy(&import_lookup_entry, mapped_bytes.data() + import_lookup_table_offset + index * sizeof(uint64_t),
                  sizeof(import_lookup_entry));

      // last entry is Null
      if(import_lookup_entry == 0)
        break;

      std::optional<std::uint32_t> import_by_name_offset =
          rva_to_file_offset(static_cast<uint32_t>(import_lookup_entry), section_header);

      if(!import_by_name_offset) [[unlikely]]
        continue;

      auto import_by_name = mapped_bytes.subspan(*import_by_name_offset);
      auto name_bytes     = import_by_name.subspan(2);

      auto it = std::ranges::find(name_bytes, std::byte{0});

      if(it == name_bytes.end()) [[unlikely]]
        break;

      auto name_length = std::distance(name_bytes.begin(), it);

      names.emplace_back(reinterpret_cast<const char*>(name_bytes.data()), static_cast<std::size_t>(name_length));
    }
  };

  return std::visit(
      [&](auto&& header) -> std::vector<Import> {
        ImageDataDirectory& import_dir = header.ImageDataDirectory[(uint8_t)DataDirectoryIndex::Import];

        std::size_t descriptor_count = import_dir.Size / sizeof(ImageImportDescriptor);

        std::vector<Import> imports;
        imports.resize(descriptor_count);

        if(import_dir.VirtualAddress == 0 && import_dir.Size == 0) [[unlikely]]
          return {};

        for(const auto& section : sections)
        {
          std::optional<uint32_t> file_offset = rva_to_file_offset(import_dir.VirtualAddress, section.Header);

          if(!file_offset)
            continue;

          std::span<const std::byte> import_data = mapped_bytes.subspan(*file_offset, import_dir.Size);

          for(std::size_t index = 0; index < descriptor_count; ++index)
          {
            std::memcpy(&imports[index].Descriptor, import_data.data() + index * sizeof(ImageImportDescriptor),
                        sizeof(ImageImportDescriptor));

            auto dll_name_offset = rva_to_file_offset(imports[index].Descriptor.Name, section.Header);

            if(!dll_name_offset)
              continue;

            auto dll_name_bytes = mapped_bytes.subspan(*dll_name_offset);

            auto it = std::ranges::find(dll_name_bytes, std::byte{0});

            // bad Image no null terminator
            if(it == dll_name_bytes.end()) [[unlikely]]
              break;

            auto dll_name_length = std::distance(dll_name_bytes.begin(), it);

            imports[index].DLLName = std::string_view{reinterpret_cast<const char*>(dll_name_bytes.data()),
                                                      static_cast<std::size_t>(dll_name_length)};

            std::optional<uint32_t> import_lookup_table_offset =
                rva_to_file_offset(imports[index].Descriptor.OriginalFirstThunk, section.Header);

            if(!import_lookup_table_offset)
              continue;

            parse_import_names(*import_lookup_table_offset, section.Header, imports[index].Names);

            if(imports[index].Descriptor.OriginalFirstThunk == 0 && imports[index].Descriptor.TimeDateStamp == 0
               && imports[index].Descriptor.ForwardChain == 0 && imports[index].Descriptor.Name == 0
               && imports[index].Descriptor.FirstThunk == 0)
              break;
          }
        }

        return imports;
      },
      optional_header);
}
