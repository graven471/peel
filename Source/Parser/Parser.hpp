#pragma once

#include <span>
#include <vector>

#include "Core/Error.hpp"
#include "PETypes.hpp"

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
