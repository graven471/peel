#pragma once

#include <cstdint>
#include <array>
#include <string>

// "MZ"
constexpr uint16_t PE_MAGIC = 0x5A4D;
// "PE\0\0"
constexpr std::array<std::byte, 4> SIGNATURE = { std::byte{ 0x50 }, std::byte{ 0x45 }, std::byte{ 0x00 }, std::byte{ 0x00 } };

struct DosHeader {                          // DOS .EXE header
	uint16_t   Magic;                     // Magic number
	uint16_t   Cblp;                      // Bytes on last page of file
	uint16_t   Cp;                        // Pages in file
	uint16_t   Crlc;                      // Relocations
	uint16_t   CparHdr;                   // Size of header in paragraphs
	uint16_t   MinAlloc;                  // Minimum extra paragraphs needed
	uint16_t   MaxAlloc;                  // Maximum extra paragraphs needed
	uint16_t   Ss;                        // Initial (relative) SS value
	uint16_t   Sp;                        // Initial SP value
	uint16_t   CSum;                      // Checksum
	uint16_t   Ip;                        // Initial IP value
	uint16_t   Cs;                        // Initial (relative) CS value
	uint16_t   LfarLc;                    // File address of relocation table
	uint16_t   Ovno;                      // Overlay number
	std::array<uint16_t, 4>    Res;       // Reserved uint16_ts
	uint16_t   Oemid;                     // OEM identifier (for e_oeminfo)
	uint16_t   OemInfo;                   // OEM information; e_oemid specific
	std::array<uint16_t, 10>   Res2;       // Reserved uint16_ts
	int32_t   LfaNew;                     // File address of new exe header
};

static_assert(sizeof(DosHeader) == 64 && "Dos Header must needs to be 64 bytes");
static_assert(offsetof(DosHeader, LfaNew) == 0x3C);

enum class MachineType : uint16_t {
	MACHINE_UNKNOWN = 0x00,
	MACHINE_ALPHA = 0x184,
	MACHINE_ALPHA64 = 0x284,
	MACHINE_AMD64 = 0x8664,
	MACHINE_ARM = 0x1c0,
	MACHINE_ARM64 = 0xaa64,
	MACHINE_I386 = 0x14c,
	MACHINE_RISCV32 = 0x5032,
	MACHINE_RISCV64 = 0x5064,
	MACHINE_RISCV128 = 0x5128,

	// OG
	MACHINE_POWERPC = 0x1f0
};

enum class FileCharacteristics : uint16_t {
	//Image only, Windows CE, and Microsoft Windows NT and later.
	// This indicates that the file does not contain base relocations
	// and must therefore be loaded at its preferred base address.
	RelocsStripped = 0x0001,
	// Image only, indicates that the image file is valid and can be run
	ExecutableImage = 0x0002,
	// deprecated should be 0
	LineNumsStripped = 0x0004,
	// deprecated should be 0
	LocalSymsStripped = 0x0008,
	// Obsolete. Aggressively trim working set. This flag is deprecated for Windows 2000 and later and must be zero. 
	AggressiveWsTrim = 0x0010,
	// application can handle > 2 GB addresses.
	LargeAddressAware = 0x0020,
	// deprecated should be 0
	BytesReversedLo = 0x0080,
	// machine is based on 32-bit-word architecture
	Bit32Machine = 0x0100,
	// debugging information is removed from image file
	DebugStripped = 0x0200,
	// If the image is on removable media, fully load it and copy it to the swap file.
	RemovableRunFromSwap = 0x0400,
	// If the image is on network media, fully load it and copy it to the swap file.
	NetRunFromSwap = 0x0800,
	// image is a system file not a user program.
	System = 0x1000,
	// image file is a dynamic-link-library (DLL). such files files are considered executable files
	// for almost all purposes, although they cannot be directly run. 
	Dll = 0x2000,
	// The file should be run only on a uniprocessor machine.
	UpSystemOnly = 0x4000,
	// deprecated should be 0
	BytesReversedHi = 0x8000,
};

struct FileHeader {
	MachineType                Machine;
	uint16_t                   NumberOfSections;
	uint32_t                   TimeDateStamp;
	uint32_t                   PointerToSymbolTable;
	uint32_t                   NumberOfSymbols;
	uint16_t                   SizeOfOptionalHeader;
	FileCharacteristics        Characteristics;
};

static_assert(sizeof(FileHeader) == 20, "File header must be exactly 20 bytes");

enum class PEFormat : uint16_t {
	// 32-bit can only use 32-bit address space no rax, rdi, rsi stuff only e prefix eax, edi etc
	PE32 = 0x10b,
	// 64-bit image can use 64-bit address space and image size can be 2 GB
	PE64 = 0x20b
};

struct OptionalHeader {
	// 64-bit image or 32-bit
	PEFormat Format;
	uint8_t MajorLinkerVersion;
	uint8_t MinorLinkerVersion;
	// size of the .text section combined
	uint32_t SizeOfCode;
	// size of .data section
	uint32_t SizeOfInitializedData;
	// size of .bss section
	uint32_t SizeOfUninitializedData;
	// RVA (relative virtual address) of WinMainCRTStartup or similar in linux terms its same as _start
	// ImageBase + AddressOfEntryPoint
	uint32_t AddressOfEntryPoint;
	uint32_t BaseOfCode;
	uint32_t BaseOfData;
};

static_assert(sizeof(OptionalHeader) == 28 && "Optional Header Must be 28 bytes");

constexpr std::string_view MachineToStringView(MachineType Type) noexcept
{
	switch (Type)
	{
	case MachineType::MACHINE_UNKNOWN:  return "Unknown";
	case MachineType::MACHINE_ALPHA:    return "Alpha";
	case MachineType::MACHINE_ALPHA64:  return "Alpha64";
	case MachineType::MACHINE_AMD64:    return "AMD64";
	case MachineType::MACHINE_ARM:      return "ARM";
	case MachineType::MACHINE_ARM64:    return "ARM64";
	case MachineType::MACHINE_I386:     return "I386";
	case MachineType::MACHINE_RISCV32:  return "RISC-V 32";
	case MachineType::MACHINE_RISCV64:  return "RISC-V 64";
	case MachineType::MACHINE_RISCV128: return "RISC-V 128";
	case MachineType::MACHINE_POWERPC:  return "PowerPC";

	default:
		std::unreachable();
	}

}

constexpr std::string_view PEFormatToStringView(PEFormat Format) noexcept {
	switch (Format) {
	case PEFormat::PE32: return "PE32";
	case PEFormat::PE64: return "PE64 aka PE32_PLUS";

	default:
		std::unreachable();
	};
}

constexpr std::string_view CharacteristicsToStringView(FileCharacteristics Flag) noexcept
{
	switch (Flag)
	{
	case FileCharacteristics::RelocsStripped:
		return "IMAGE_FILE_RELOCS_STRIPPED";
	case FileCharacteristics::ExecutableImage:
		return "IMAGE_FILE_EXECUTABLE_IMAGE";
	case FileCharacteristics::LineNumsStripped:
		return "IMAGE_FILE_LINE_NUMS_STRIPPED";
	case FileCharacteristics::LocalSymsStripped:
		return "IMAGE_FILE_LOCAL_SYMS_STRIPPED";
	case FileCharacteristics::AggressiveWsTrim:
		return "IMAGE_FILE_AGGRESSIVE_WS_TRIM";
	case FileCharacteristics::LargeAddressAware:
		return "IMAGE_FILE_LARGE_ADDRESS_AWARE";
	case FileCharacteristics::BytesReversedLo:
		return "IMAGE_FILE_BYTES_REVERSED_LO";
	case FileCharacteristics::Bit32Machine:
		return "IMAGE_FILE_32BIT_MACHINE";
	case FileCharacteristics::DebugStripped:
		return "IMAGE_FILE_DEBUG_STRIPPED";
	case FileCharacteristics::RemovableRunFromSwap:
		return "IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP";
	case FileCharacteristics::NetRunFromSwap:
		return "IMAGE_FILE_NET_RUN_FROM_SWAP";
	case FileCharacteristics::System:
		return "IMAGE_FILE_SYSTEM";
	case FileCharacteristics::Dll:
		return "IMAGE_FILE_DLL";
	case FileCharacteristics::UpSystemOnly:
		return "IMAGE_FILE_UP_SYSTEM_ONLY";
	case FileCharacteristics::BytesReversedHi:
		return "IMAGE_FILE_BYTES_REVERSED_HI";
	}

	return "UNKNOWN";
}

inline std::string CharacteristicsToString(std::uint16_t Characteristics)
{
	std::string Result;

	constexpr std::array Flags{
		FileCharacteristics::RelocsStripped,
		FileCharacteristics::ExecutableImage,
		FileCharacteristics::LineNumsStripped,
		FileCharacteristics::LocalSymsStripped,
		FileCharacteristics::AggressiveWsTrim,
		FileCharacteristics::LargeAddressAware,
		FileCharacteristics::BytesReversedLo,
		FileCharacteristics::Bit32Machine,
		FileCharacteristics::DebugStripped,
		FileCharacteristics::RemovableRunFromSwap,
		FileCharacteristics::NetRunFromSwap,
		FileCharacteristics::System,
		FileCharacteristics::Dll,
		FileCharacteristics::UpSystemOnly,
		FileCharacteristics::BytesReversedHi,
	};

	for (const auto Flag : Flags)
	{
		const uint16_t Value = static_cast<uint16_t>(Flag);

		if ((Characteristics & Value) == 0)
			continue;

		if (!Result.empty())
			Result += " | ";

		Result += CharacteristicsToStringView(Flag);
	}

	return Result;
}

constexpr bool HasFlag(uint16_t Value, FileCharacteristics Flag) noexcept {
	return (Value & static_cast<uint16_t>(Flag)) != 0;
}
