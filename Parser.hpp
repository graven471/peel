#pragma once

#include <span>
#include "Error.hpp"
#include "PETypes.hpp"

class PEParser {
public:
	explicit PEParser(std::span<const std::byte> InMappedBytes) : MappedBytes(InMappedBytes) {};

	PeelResult<void> Parse();

private:
	PeelResult<DosHeader> ParseDosHeader(std::span<const std::byte> InDosHeader);
	PeelResult<FileHeader> ParseFileHeader(std::span<const std::byte> InImageHeader);
	PeelResult<OptionalHeader> ParseOptionalHeader(std::span<const std::byte> InOptionalHeader);

	std::span<const std::byte> MappedBytes{};
};
