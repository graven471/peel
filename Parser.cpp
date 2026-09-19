#include "Parser.hpp"

#include <algorithm>
#include <ranges>
#include <vector>

#include "PETypes.hpp"

/*
the format is defined like this:

IMAGE_DOS_HEADER: start: 0 end: 64
important thing there is MAGIC ("MZ") and LfaNew at offset 60 (0x3C) in
DosHeader
// this LfaNew tells us where to find signature (offset) "PE\0\0" for every
valid PE file there exists a signature
// "PE\0\0"

DOS_STUB
// then there is a DOS_STUB which is a char(s) that says "This program cannot be
run in DOS mode"

// SIGNATURE: size: 4 bytes
// "PE\0\0"

// FileHeader: start: LfaNew + SIGNATURE_SIZE end: (LfaNew +
SIGNATURE_SIZE)..FILE_HEADER_SIZE (20)
// this header stores machine type, num of sections, num of symbols, size of
optional header and Characteristics
// which contains flags that indicates attribute of the image or object file
e.g. (dll, executable etc)

// Optional Header: start: DosHeader::LfaNew + SIGNATURE.size() + FileHeader
size end: FileHeader::SizeOfOptionalHeader
// it is required image files and can be optional for object files
// MAGIC -> 0x10b = PE32 (x86)
// MAGIC -> 0x20b = PE32+ (AMD64) can use 64 bit address space and limiting
image size to 2 GB
// SizeOfCode -> size of .text section
// SizeOfData -> size of .data section
*/

// return void for now later return a struct that represent full parsed valid PE
// file data so that i can feed to GUI or CLI or dumb etc
[[nodiscard]] PeelResult<PEImage> PEParser::Parse() {
  PeelResult<ImageDosHeader> DosHeader =
      ParseDosHeader(MappedBytes.subspan(0, sizeof(ImageDosHeader)));

  if (!DosHeader) return std::unexpected(DosHeader.error());

  // [PE_SIGNATURE | FileHeader (20 bytes) | OptionalHeader]
  PeelResult<ImageFileHeader> FileHeader = ParseFileHeader(MappedBytes.subspan(
      DosHeader->LfaNew, sizeof(ImageFileHeader) + SIGNATURE.size()));

  if (!FileHeader) return std::unexpected(FileHeader.error());

  // every image file has an optional header header that provides info to
  // loader. this header is optional in object files, for image files this
  // header is required it's size is not fixed FileHeader::SizeOfOptionalHeader
  // must be used to validate
  const std::size_t OptionalHeaderOffset =
      DosHeader->LfaNew + SIGNATURE.size() + sizeof(::ImageFileHeader);

  PeelResult<ImageOptionalHeader> OptionalHeader =
      ParseOptionalHeader(MappedBytes.subspan(
          OptionalHeaderOffset, FileHeader->SizeOfOptionalHeader));

  if (!OptionalHeader) return std::unexpected(OptionalHeader.error());

  std::println("Machine: \t {}", MachineToStringView(FileHeader->Machine));

  ImageNTHeaders NTHeaders{
      .FileHeader = *FileHeader,
      .OptionalHeader = *OptionalHeader,
  };

  const std::size_t SectionHeaderOffset =
      DosHeader->LfaNew + sizeof(SIGNATURE) + sizeof(ImageFileHeader) +
      FileHeader->SizeOfOptionalHeader;

  PeelResult<std::vector<ImageSection>> Sections = ParseSectionHeader(
      MappedBytes.subspan(SectionHeaderOffset, FileHeader->NumberOfSections *
                                                   sizeof(ImageSectionHeader)),
      FileHeader->NumberOfSections);

  if (!Sections) return std::unexpected(Sections.error());

  std::vector<Import> Imports =
      ParseImportSection(*Sections, NTHeaders.OptionalHeader);

  return PEImage{.DosHeader = *DosHeader,
                 .NTHeaders = NTHeaders,
                 .SectionHeaders = std::move(*Sections),
                 .Imports = std::move(Imports)};
}

PeelResult<ImageDosHeader> PEParser::ParseDosHeader(
    std::span<const std::byte> InDosHeader) {
  ImageDosHeader Header{};
  std::memcpy(&Header, InDosHeader.data(), sizeof(ImageDosHeader));

  if (Header.Magic != PE_MAGIC) {
    return std::unexpected(PeelError{PeelErrorCode::PEEL_INVALID_MAGIC});
  }

  return Header;
}

PeelResult<ImageFileHeader> PEParser::ParseFileHeader(
    std::span<const std::byte> InImageHeader) {
  if (!std::ranges::equal(SIGNATURE,
                          InImageHeader.subspan(0, SIGNATURE.size()))) {
    return std::unexpected(PeelError{PeelErrorCode::PEEL_INVALID_SIGNATURE});
  }

  ImageFileHeader Header{};
  // skip 4 bytes of signature
  std::memcpy(&Header, InImageHeader.data() + SIGNATURE.size(),
              sizeof(ImageFileHeader));

  return Header;
}

PeelResult<ImageOptionalHeader> PEParser::ParseOptionalHeader(
    std::span<const std::byte> InOptionalHeader) {
  uint16_t Magic{0};

  std::memcpy(&Magic, InOptionalHeader.subspan(0, sizeof(uint16_t)).data(),
              sizeof(uint16_t));

  switch (static_cast<PEFormat>(Magic)) {
    case PEFormat::PE32: {
      ImageOptionalHeader32 Header{};
      std::memcpy(&Header, InOptionalHeader.data(),
                  sizeof(ImageOptionalHeader32));
      return ImageOptionalHeader{Header};
    }

    case PEFormat::PE64: {
      ImageOptionalHeader64 Header{};
      std::memcpy(&Header, InOptionalHeader.data(),
                  sizeof(ImageOptionalHeader64));
      return ImageOptionalHeader{Header};
    }

    default:
      return std::unexpected(
          PeelError{PeelErrorCode::PEEL_UNSUPPORTED_PE_VERSION});
  }
}

PeelResult<std::vector<ImageSection>> PEParser::ParseSectionHeader(
    std::span<const std::byte> InSectionHeader, uint32_t TotalSections) {
  std::vector<ImageSection> Sections(TotalSections);

  for (std::size_t Index = 0; Index < TotalSections; Index++) {
    const uint32_t Offset = Sections[Index].Header.PointerToRawData;
    const uint32_t Size = Sections[Index].Header.SizeOfRawData;

    if (Offset > MappedBytes.size() || Size > MappedBytes.size() - Offset)
        [[unlikely]] {
      return std::unexpected(PeelError{PeelErrorCode::PEEL_INVALID_SECTION});
    }

    std::memcpy(&Sections[Index].Header,
                InSectionHeader.data() + Index * sizeof(ImageSectionHeader),
                sizeof(ImageSectionHeader));

    // view of bytes on disk
    Sections[Index].Data = MappedBytes.subspan(Offset, Size);

    // debug
    std::println("Name: \t {}{}{}{}{}{}{}{}", Sections[Index].Header.Name[0],
                 Sections[Index].Header.Name[1], Sections[Index].Header.Name[2],
                 Sections[Index].Header.Name[3], Sections[Index].Header.Name[4],
                 Sections[Index].Header.Name[5], Sections[Index].Header.Name[6],
                 Sections[Index].Header.Name[7]);

    std::println("Addr: \t 0x{:02X}", Sections[Index].Header.VirtualAddress);

    std::println("Flag: \t {}",
                 ImageSectionFlagsToString(
                     (uint32_t)Sections[Index].Header.Characteristics));
  }

  return Sections;
}

// TODO: not a fastest or solid implementation comeback and improve it
std::vector<Import> PEParser::ParseImportSection(
    std::span<const ImageSection> Sections,
    ImageOptionalHeader& OptionalHeader) {
  auto ParseFunctionNames = [&](uint32_t ImportLookupTableOffset,
                                const ImageSectionHeader& SectionHeader,
                                std::vector<std::string_view>& Names) {
    for (std::size_t Index = 0;; Index++) {
      // ILT entry -> IMAGE_IMPORT_BY_NAME (hint, name)

      // todo: handle PE32 in that case this is uint32_t
      uint64_t Entry;

      std::memcpy(&Entry,
                  MappedBytes.data() + ImportLookupTableOffset +
                      Index * sizeof(uint64_t),
                  sizeof(Entry));

      // last entry is Null
      if (Entry == 0) break;

      std::optional<uint32_t> ImportByNameOffset =
          RvaToFileOffset(static_cast<uint32_t>(Entry), SectionHeader);

      if (!ImportByNameOffset) [[unlikey]]
        continue;

      auto ImportByName = MappedBytes.subspan(*ImportByNameOffset);

      // skip hint first 2 bytes
      auto NameBytes = ImportByName.subspan(2);

      auto It = std::ranges::find(NameBytes, std::byte{0});

      // bad Image no null terminator
      if (It == NameBytes.end()) [[unlikely]]
        break;

      auto NameLength = std::distance(NameBytes.begin(), It);

      Names.emplace_back(reinterpret_cast<const char*>(NameBytes.data()),
                         static_cast<std::size_t>(NameLength));
    }
  };

  return std::visit(
      [&](auto&& Header) -> std::vector<Import> {
        ImageDataDirectory& ImportDir =
            Header.ImageDataDirectory[(uint8_t)DataDirectoryIndex::Import];

        std::size_t DescriptorCount =
            ImportDir.Size / sizeof(ImageImportDescriptor);

        std::vector<Import> Imports;
        Imports.resize(DescriptorCount);

        if (ImportDir.VirtualAddress == 0 && ImportDir.Size == 0) [[unlikely]]
          return {};

        for (const auto& Section : Sections) {
          std::optional<uint32_t> FileOffset =
              RvaToFileOffset(ImportDir.VirtualAddress, Section.Header);

          if (!FileOffset) continue;

          std::span<const std::byte> ImportData =
              MappedBytes.subspan(*FileOffset, ImportDir.Size);

          for (std::size_t Index = 0; Index < DescriptorCount; ++Index) {
            std::memcpy(
                &Imports[Index].Descriptor,
                ImportData.data() + Index * sizeof(ImageImportDescriptor),
                sizeof(ImageImportDescriptor));

            auto DLLNameOffset =
                RvaToFileOffset(Imports[Index].Descriptor.Name, Section.Header);

            if (!DLLNameOffset) continue;

            auto DLLNameBytes = MappedBytes.subspan(*DLLNameOffset);

            auto It = std::ranges::find(DLLNameBytes, std::byte{0});

            // bad Image no null terminator
            if (It == DLLNameBytes.end()) [[unlikely]]
              break;

            auto DLLNameLength = std::distance(DLLNameBytes.begin(), It);

            Imports[Index].DLLName = std::string_view{
                reinterpret_cast<const char*>(DLLNameBytes.data()),
                static_cast<std::size_t>(DLLNameLength)};

            std::optional<uint32_t> ImportLookupTableOffset = RvaToFileOffset(
                Imports[Index].Descriptor.OriginalFirstThunk, Section.Header);

            if (!ImportLookupTableOffset) continue;

            ParseFunctionNames(*ImportLookupTableOffset, Section.Header,
                               Imports[Index].Names);

            if (Imports[Index].Descriptor.OriginalFirstThunk == 0 &&
                Imports[Index].Descriptor.TimeDateStamp == 0 &&
                Imports[Index].Descriptor.ForwardChain == 0 &&
                Imports[Index].Descriptor.Name == 0 &&
                Imports[Index].Descriptor.FirstThunk == 0)
              break;
          }
        }

        return Imports;
      },

      OptionalHeader);
}
