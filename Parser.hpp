#pragma once

#include "Error.hpp"
#include "PETypes.hpp"
#include <span>
#include <vector>

struct PEImage {
  ImageDosHeader DosHeader;
  ImageNTHeaders NTHeaders;
  std::vector<ImageSectionHeader> SectionHeaders;
};

class PEParser {
public:
  explicit PEParser(std::span<const std::byte> InMappedBytes)
      : MappedBytes(InMappedBytes) {};

  PeelResult<PEImage> Parse();

private:
  PeelResult<ImageDosHeader>
  ParseDosHeader(std::span<const std::byte> InDosHeader);
  PeelResult<ImageFileHeader>
  ParseFileHeader(std::span<const std::byte> InImageHeader);
  PeelResult<ImageOptionalHeader>
  ParseOptionalHeader(std::span<const std::byte> InOptionalHeader);

  PeelResult<std::vector<ImageSectionHeader>>
  ParseSectionHeader(std::span<const std::byte> InSectionHeader,
                     uint32_t TotalSections);

  std::span<const std::byte> MappedBytes{};
};
