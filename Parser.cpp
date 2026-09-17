#include "Parser.hpp"
#include "PETypes.hpp"
#include <ranges>
#include <algorithm>

/*
the format is defined like this:

IMAGE_DOS_HEADER: start: 0 end: 64
important thing there is MAGIC ("MZ") and LfaNew at offset 60 (0x3C) in DosHeader
// this LfaNew tells us where to find signature (offset) "PE\0\0" for every valid PE file there exists a signature
// "PE\0\0"

DOS_STUB
// then there is a DOS_STUB which is a char(s) that says "This program cannot be run in DOS mode"

// SIGNATURE: size: 4 bytes
// "PE\0\0"

// FileHeader: start: LfaNew + SIGNATURE_SIZE end: (LfaNew + SIGNATURE_SIZE)..FILE_HEADER_SIZE (20)
// this header stores machine type, num of sections, num of symbols, size of optional header and Characteristics
// which contains flags that indicates attribute of the image or object file e.g. (dll, executable etc)

// Optional Header: start: DosHeader::LfaNew + SIGNATURE.size() + FileHeader size end: FileHeader::SizeOfOptionalHeader
// it is required image files and can be optional for object files
// MAGIC -> 0x10b = PE32 (x86)
// MAGIC -> 0x20b = PE32+ (AMD64) can use 64 bit address space and limiting image size to 2 GB
// SizeOfCode -> size of .text section
// SizeOfData -> size of .data section
*/

// return void for now later return a struct that represent full parsed valid PE file data so that
// i can feed to GUI or CLI or dumb etc
PeelResult<void> PEParser::Parse() {
	PeelResult<DosHeader> DosHeaderResult = ParseDosHeader(MappedBytes.subspan(0, sizeof(DosHeader)));

	if (!DosHeaderResult) return std::unexpected(DosHeaderResult.error());

	// [PE_SIGNATURE | FileHeader (20 bytes) | OptionalHeader]
	PeelResult<FileHeader> FileHeaderResult = ParseFileHeader(MappedBytes.subspan(DosHeaderResult->LfaNew, sizeof(FileHeader) + SIGNATURE.size()));

	if (!FileHeaderResult) return std::unexpected(FileHeaderResult.error());

	// every image file has an optional header header that provides info to loader.
	// this header is optional in object files, for image files this header is required
	// it's size is not fixed FileHeader::SizeOfOptionalHeader must be used to validate
	if (HasFlag((uint16_t)FileHeaderResult->Characteristics, FileCharacteristics::ExecutableImage)) {
		std::size_t OptionalHeaderOffset = DosHeaderResult->LfaNew + SIGNATURE.size() + sizeof(::FileHeader);

		PeelResult<OptionalHeader> OptionalHeaderResult = ParseOptionalHeader(MappedBytes.subspan(OptionalHeaderOffset, FileHeaderResult->SizeOfOptionalHeader));

		if (!OptionalHeaderResult) return std::unexpected(OptionalHeaderResult.error());
	}

	return {};
}

PeelResult<DosHeader> PEParser::ParseDosHeader(std::span<const std::byte> InDosHeader) {
	DosHeader Header{};
	std::memcpy(&Header, InDosHeader.data(), sizeof(DosHeader));

	std::println("Magic: {}, Checksum: {}, signature: {}", Header.Magic, Header.CSum, Header.LfaNew);

	if (Header.Magic != PE_MAGIC) {
		return std::unexpected(PeelError{ PeelErrorCode::PEEL_INVALID_MAGIC });
	}

	return Header;
}

PeelResult<FileHeader> PEParser::ParseFileHeader(std::span<const std::byte> InImageHeader) {
	if (!std::ranges::equal(SIGNATURE, InImageHeader.subspan(0, SIGNATURE.size()))) {
		return std::unexpected(PeelError{ PeelErrorCode::PEEL_INVALID_SIGNATURE });
	}

	FileHeader Header{};
	// skip 4 bytes of signature
	std::memcpy(&Header, InImageHeader.data() + SIGNATURE.size(), sizeof(FileHeader));

	//===========debugging==========
	std::println("Machine: {}, Sections: {}, Symbols: {}, OptionalHeaders size: {}, ImageType: {}",
		MachineToStringView(Header.Machine), Header.NumberOfSections, Header.NumberOfSymbols, Header.SizeOfOptionalHeader, CharacteristicsToString((uint16_t)Header.Characteristics));

	return Header;
}

PeelResult<OptionalHeader> PEParser::ParseOptionalHeader(std::span<const std::byte> InOptionalHeader) {
	OptionalHeader Header{};
	std::memcpy(&Header, InOptionalHeader.data(), sizeof(OptionalHeader));

	if (Header.Format != PEFormat::PE32 && Header.Format != PEFormat::PE64) {
		return std::unexpected(PeelError{ PeelErrorCode::PEEL_UNSUPPORTED_PE_VERSION });
	}

	//=============debugging=================
	std::println("Format: {}, Code size: {}, Initialized Data size: {}, Unitialized Data size: {}, Base Of Code: {}, Base Of Data: {}, Entry Point: 0x{:02X}", PEFormatToStringView(Header.Format), Header.SizeOfCode, Header.SizeOfInitializedData, Header.SizeOfInitializedData, Header.BaseOfCode, Header.BaseOfData, Header.AddressOfEntryPoint);

	return Header;
}

/*
*    uint32_t Signature;          // "PE\0\0"
	IMAGE_FILE_HEADER FileHeader;
	IMAGE_OPTIONAL_HEADER64 OptionalHeader;
*/