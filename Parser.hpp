#pragma once

#include <span>
#include <vector>

#include "Error.hpp"
#include "PETypes.hpp"

struct ImageSection {
  ImageSectionHeader Header;

  // Header.PointerToRawData..Header.SizeOfRawData raw bytes belonging to this
  // section in the PE file
  std::span<const std::byte> Data;
};

struct Import {
  ImageImportDescriptor Descriptor;
  std::string_view DLLName{};
  std::vector<std::string_view> Names{};
};

struct PEImage {
  ImageDosHeader DosHeader;
  ImageNTHeaders NTHeaders;
  std::vector<ImageSection> SectionHeaders;
  std::vector<Import> Imports;
};

class PEParser {
 public:
  explicit PEParser(std::span<const std::byte> InMappedBytes)
      : MappedBytes(InMappedBytes) {};

  [[nodiscard]] PeelResult<PEImage> Parse();

 private:
  PeelResult<ImageDosHeader> ParseDosHeader(
      std::span<const std::byte> InDosHeader);
  PeelResult<ImageFileHeader> ParseFileHeader(
      std::span<const std::byte> InImageHeader);
  PeelResult<ImageOptionalHeader> ParseOptionalHeader(
      std::span<const std::byte> InOptionalHeader);

  PeelResult<std::vector<ImageSection>> ParseSectionHeader(
      std::span<const std::byte> InSectionHeader, uint32_t TotalSections);

  std::vector<Import> ParseImportSection(std::span<const ImageSection> Sections,
                                         ImageOptionalHeader& OptionalHeader);

  std::span<const std::byte> MappedBytes{};
};
