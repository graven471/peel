#pragma once

#include "Error.hpp"
#include "PETypes.hpp"
#include <span>
#include <vector>

struct ImageSection {
  ImageSectionHeader Header;

  // Header.PointerToRawData..Header.SizeOfRawData raw bytes belonging to this
  // section in the PE file
  std::span<const std::byte> Data;
};

struct PEImage {
  ImageDosHeader DosHeader;
  ImageNTHeaders NTHeaders;
  std::vector<ImageSection> SectionHeaders;
};

class PEParser {
public:
  explicit PEParser(std::span<const std::byte> InMappedBytes)
      : MappedBytes(InMappedBytes) {};

  [[nodiscard]] PeelResult<PEImage> Parse();

private:
  PeelResult<ImageDosHeader>
  ParseDosHeader(std::span<const std::byte> InDosHeader);
  PeelResult<ImageFileHeader>
  ParseFileHeader(std::span<const std::byte> InImageHeader);
  PeelResult<ImageOptionalHeader>
  ParseOptionalHeader(std::span<const std::byte> InOptionalHeader);

  PeelResult<std::vector<ImageSection>>
  ParseSectionHeader(std::span<const std::byte> InSectionHeader,
                     uint32_t TotalSections);

  std::span<const std::byte> MappedBytes{};
};
