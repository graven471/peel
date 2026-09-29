#pragma once

#include <span>
#include <vector>

#include "Core/Error.hpp"
#include "PETypes.hpp"

class PEParser
{
public:
  explicit PEParser(std::span<const std::byte> mapped_bytes)
      : mapped_bytes(mapped_bytes) {};

  [[nodiscard]] PeelResult<PEImage> parse();

private:
  PeelResult<ImageDosHeader>  parse_dos_header(std::span<const std::byte> dos_header);
  PeelResult<ImageFileHeader> parse_file_header(std::span<const std::byte> image_header);

  PeelResult<ImageOptionalHeader> parse_optional_header(std::span<const std::byte> optional_header);
  PeelResult<std::vector<ImageSection>> parse_section_headers(std::span<const std::byte> section_header, std::uint32_t section_count);

  std::vector<Import> parse_imports(std::span<const ImageSection> sections, ImageOptionalHeader& optional_header);

  std::span<const std::byte> mapped_bytes{};
};