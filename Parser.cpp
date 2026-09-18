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
PeelResult<PEImage> PEParser::Parse() {
  PeelResult<ImageDosHeader> DosHeader =
      ParseDosHeader(MappedBytes.subspan(0, sizeof(ImageDosHeader)));

  if (!DosHeader)
    return std::unexpected(DosHeader.error());

  // [PE_SIGNATURE | FileHeader (20 bytes) | OptionalHeader]
  PeelResult<ImageFileHeader> FileHeader = ParseFileHeader(MappedBytes.subspan(
      DosHeader->LfaNew, sizeof(ImageFileHeader) + SIGNATURE.size()));

  if (!FileHeader)
    return std::unexpected(FileHeader.error());

  // every image file has an optional header header that provides info to
  // loader. this header is optional in object files, for image files this
  // header is required it's size is not fixed FileHeader::SizeOfOptionalHeader
  // must be used to validate
  const std::size_t OptionalHeaderOffset =
      DosHeader->LfaNew + SIGNATURE.size() + sizeof(::ImageFileHeader);

  PeelResult<ImageOptionalHeader> OptionalHeader =
      ParseOptionalHeader(MappedBytes.subspan(
          OptionalHeaderOffset, FileHeader->SizeOfOptionalHeader));

  if (!OptionalHeader)
    return std::unexpected(OptionalHeader.error());

  std::println("Machine: \t {}", MachineToStringView(FileHeader->Machine));

  std::visit(
      [](auto &&Header) {
        std::println("Format: \t {}", PEFormatToStringView(Header.Format));

        std::println("Subsystem: \t {}",
                     ImageSubSystemToStringView(Header.Subsystem));
      },
      *OptionalHeader);

  ImageNTHeaders NTHeaders{
      .FileHeader = *FileHeader,
      .OptionalHeader = *OptionalHeader,
  };

  const std::size_t SectionHeaderOffset =
      DosHeader->LfaNew + sizeof(SIGNATURE) + sizeof(ImageFileHeader) +
      FileHeader->SizeOfOptionalHeader;

  PeelResult<std::vector<ImageSectionHeader>> SectionHeaders =
      ParseSectionHeader(MappedBytes.subspan(SectionHeaderOffset,
                                             FileHeader->NumberOfSections *
                                                 sizeof(ImageSectionHeader)),
                         FileHeader->NumberOfSections);

  if (!SectionHeaders)
    return std::unexpected(SectionHeaders.error());

  return PEImage{.DosHeader = *DosHeader,
                 .NTHeaders = NTHeaders,
                 .SectionHeaders = std::move(*SectionHeaders)};
}

PeelResult<ImageDosHeader>
PEParser::ParseDosHeader(std::span<const std::byte> InDosHeader) {
  ImageDosHeader Header{};
  std::memcpy(&Header, InDosHeader.data(), sizeof(ImageDosHeader));

  if (Header.Magic != PE_MAGIC) {
    return std::unexpected(PeelError{PeelErrorCode::PEEL_INVALID_MAGIC});
  }

  return Header;
}

PeelResult<ImageFileHeader>
PEParser::ParseFileHeader(std::span<const std::byte> InImageHeader) {
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

PeelResult<ImageOptionalHeader>
PEParser::ParseOptionalHeader(std::span<const std::byte> InOptionalHeader) {
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

PeelResult<std::vector<ImageSectionHeader>>
PEParser::ParseSectionHeader(std::span<const std::byte> InSectionHeader,
                             uint32_t TotalSections) {
  // todo: do validation
  std::vector<ImageSectionHeader> Sections(TotalSections);

  std::memcpy(Sections.data(), InSectionHeader.data(), InSectionHeader.size());

  for (const auto &Header : Sections) {
    std::println("Name: \t {}{}{}{}{}{}{}{}", Header.Name[0], Header.Name[1],
                 Header.Name[2], Header.Name[3], Header.Name[4], Header.Name[5],
                 Header.Name[6], Header.Name[7]);

    std::println("Addr: \t 0x{:02X}", Header.VirtualAddress);

    std::println("Flag: \t {}",
                 ImageSectionFlagsToString((uint32_t)Header.Characteristics));
  }

  return Sections;
}
