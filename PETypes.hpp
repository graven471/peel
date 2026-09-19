#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <variant>

// "MZ"
constexpr uint16_t PE_MAGIC = 0x5A4D;
// "PE\0\0"
constexpr std::array<std::byte, 4> SIGNATURE = {
	std::byte{ 0x50 }, std::byte{ 0x45 }, std::byte{ 0x00 }, std::byte{ 0x00 } };

constexpr std::size_t NUMBEROF_DIRECTORY_ENTRIES = 16;

struct ImageDosHeader {          // DOS .EXE header
	uint16_t Magic;                // Magic number
	uint16_t Cblp;                 // Bytes on last page of file
	uint16_t Cp;                   // Pages in file
	uint16_t Crlc;                 // Relocations
	uint16_t CparHdr;              // Size of header in paragraphs
	uint16_t MinAlloc;             // Minimum extra paragraphs needed
	uint16_t MaxAlloc;             // Maximum extra paragraphs needed
	uint16_t Ss;                   // Initial (relative) SS value
	uint16_t Sp;                   // Initial SP value
	uint16_t CSum;                 // Checksum
	uint16_t Ip;                   // Initial IP value
	uint16_t Cs;                   // Initial (relative) CS value
	uint16_t LfarLc;               // File address of relocation table
	uint16_t Ovno;                 // Overlay number
	std::array<uint16_t, 4> Res;   // Reserved uint16_ts
	uint16_t Oemid;                // OEM identifier (for e_oeminfo)
	uint16_t OemInfo;              // OEM information; e_oemid specific
	std::array<uint16_t, 10> Res2; // Reserved uint16_ts
	int32_t LfaNew;                // File address of new exe header
};

static_assert(sizeof(ImageDosHeader) == 64 &&
	"Dos Header must needs to be 64 bytes");
static_assert(offsetof(ImageDosHeader, LfaNew) == 0x3C);

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
	// Image only, Windows CE, and Microsoft Windows NT and later.
	// This indicates that the file does not contain base relocations
	// and must therefore be loaded at its preferred base address.
	RelocsStripped = 0x0001,
	// Image only, indicates that the image file is valid and can be run
	ExecutableImage = 0x0002,
	// deprecated should be 0
	LineNumsStripped = 0x0004,
	// deprecated should be 0
	LocalSymsStripped = 0x0008,
	// Obsolete. Aggressively trim working set. This flag is deprecated for
	// Windows 2000 and later and must be zero.
	AggressiveWsTrim = 0x0010,
	// application can handle > 2 GB addresses.
	LargeAddressAware = 0x0020,
	// deprecated should be 0
	BytesReversedLo = 0x0080,
	// machine is based on 32-bit-word architecture
	Bit32Machine = 0x0100,
	// debugging information is removed from image file
	DebugStripped = 0x0200,
	// If the image is on removable media, fully load it and copy it to the swap
	// file.
	RemovableRunFromSwap = 0x0400,
	// If the image is on network media, fully load it and copy it to the swap
	// file.
	NetRunFromSwap = 0x0800,
	// image is a system file not a user program.
	System = 0x1000,
	// image file is a dynamic-link-library (DLL). such files files are considered
	// executable files
	// for almost all purposes, although they cannot be directly run.
	Dll = 0x2000,
	// The file should be run only on a uniprocessor machine.
	UpSystemOnly = 0x4000,
	// deprecated should be 0
	BytesReversedHi = 0x8000,
};

struct ImageFileHeader {
	MachineType Machine;
	uint16_t NumberOfSections;
	uint32_t TimeDateStamp;
	uint32_t PointerToSymbolTable;

	// total symbols in image
	uint32_t NumberOfSymbols;
	uint16_t SizeOfOptionalHeader;
	FileCharacteristics Characteristics;
};

static_assert(sizeof(ImageFileHeader) == 20,
	"ImageFileHeader must be exactly 20 bytes");

enum class PEFormat : uint16_t {
	// 32-bit can only use 32-bit address space no rax, rdi, rsi stuff only e
	// prefix eax, edi etc
	PE32 = 0x10b,
	// 64-bit image can use 64-bit address space and image size can be 2 GB
	PE64 = 0x20b
};

// each data directory gives address and size of a table or string that windows
// uses.
struct ImageDataDirectory {
	// RVA of the table. The RVA is the address of the table relative to the base
	// address of the image
	uint32_t VirtualAddress;
	// size in bytes
	uint32_t Size;
};

static_assert(sizeof(ImageDataDirectory) == 8);

enum class DataDirectoryIndex : uint8_t {
	// .edata section (image only) contains information about symbols that other
	// images can access through dynamic linking
	Export = 0,
	// .idata section All image files that import symbols, including virtually all
	// executable (EXE) files, have an .idata section
	Import = 1,
	Resource = 2,
	// .pdata section
	Exception = 3,
	Certificate = 4,
	BaseRelocation = 5,
	Debug = 6,
	Architecture = 7,
	GlobalPtr = 8,
	TLS = 9,
	LoadConfig = 10,
	BoundImport = 11,
	IAT = 12,
	DelayImport = 13,
	CLRRuntime = 14,
	Reserved = 15,
};

// what kind of program is this
enum class ImageSubsystemType : uint16_t {
	// an unknown system
	Unknown = 0,
	// device drivers and native windows processes
	Native = 1,
	// windows Graphical User Interface subsystem
	GUI = 2,
	// windows character system
	WindowsCUI = 3,
	// OS/2 character system
	OS2CUI = 5,
	// Posix character system
	PosixCUI = 7,
	// Native Win9x driver
	NativeWindows = 8,
	// Windows CE
	WindowsCeGUI = 9,
	// Extensible Firmware Application (EFI) application
	EFIApplication = 10,
	// An EFI driver with boot service
	EFIBootServiceDriver = 11,
	// An EFI run-time services
	EFIRuntimeDriver = 12,
	// An EFI ROM image
	EFIRom = 13,
	// XBOX
	XBOX = 14,
	// Windows boot Application
	WindowsBootApplication = 16
};

#pragma pack(push, 1)
struct ImageOptionalHeader64 {
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
	// RVA (relative virtual address) of WinMainCRTStartup or similar in Linux
	// terms its same as _start ImageBase + AddressOfEntryPoint
	uint32_t AddressOfEntryPoint;
	uint32_t BaseOfCode;

	// Windows specific fields (Image only)

	// preferred address of the first byte of image when loaded into memory;
	// for DLLs default is 0x10000000
	// for Windows CE EXEs is 0x00010000
	// for NT, 2000, XP, 95, 98 and Windows Me is 0x00400000
	uint64_t ImageBase;
	uint32_t SectionAlignment;
	uint32_t FileAlignment;
	uint16_t MajorOperatingSystemVersion;
	uint16_t MinorOperatingSystemVersion;
	uint16_t MajorImageVersion;
	uint16_t MinorImageVersion;
	uint16_t MajorSubsystemVersion;
	uint16_t MinorSubsystemVersion;
	uint32_t Win32VersionValue;
	// The size(in bytes) of the image, including all headers,
	//     as the image is loaded in memory.It must be a multiple of
	//     SectionAlignment.
	uint32_t SizeOfImage;
	uint32_t SizeOfHeaders;
	uint32_t CheckSum;
	// subsystem that is required to run this image
	ImageSubsystemType Subsystem;
	// TODO: IMAGE_DLLCHARACTERISTICS enum
	uint16_t DllCharacteristics;
	uint64_t SizeOfStackReserve;
	uint64_t SizeOfStackCommit;
	uint64_t SizeOfHeapReserve;
	uint64_t SizeOfHeapCommit;
	uint32_t LoaderFlags;
	uint32_t NumberOfRvaAndSizes;
	// size and virtual address these are all loaded into memory so that windows
	// system can use them at run time
	std::array<ImageDataDirectory, NUMBEROF_DIRECTORY_ENTRIES> ImageDataDirectory;
};

static_assert(sizeof(ImageOptionalHeader64) == 240 &&
	"Optional Header 64 Must be 240 bytes");

struct ImageOptionalHeader32 {
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
	// RVA (relative virtual address) of WinMainCRTStartup or similar in Linux
	// terms its same as _start ImageBase + AddressOfEntryPoint
	uint32_t AddressOfEntryPoint;
	uint32_t BaseOfCode;
	uint32_t BaseOfData;

	// Windows specific fields (Image only)
	uint64_t ImageBase;
	uint32_t SectionAlignment;
	uint32_t FileAlignment;
	uint16_t MajorOperatingSystemVersion;
	uint16_t MinorOperatingSystemVersion;
	uint16_t MajorImageVersion;
	uint16_t MinorImageVersion;
	uint16_t MajorSubsystemVersion;
	uint16_t MinorSubsystemVersion;
	uint32_t Win32VersionValue;
	uint32_t SizeOfImage;
	uint32_t SizeOfHeaders;
	uint32_t CheckSum;
	ImageSubsystemType Subsystem;
	uint16_t DllCharacteristics;
	uint64_t SizeOfStackReserve;
	uint64_t SizeOfStackCommit;
	uint64_t SizeOfHeapReserve;
	uint64_t SizeOfHeapCommit;
	uint32_t LoaderFlags;
	uint32_t NumberOfRvaAndSizes;
	std::array<ImageDataDirectory, NUMBEROF_DIRECTORY_ENTRIES> ImageDataDirectory;
};

static_assert(sizeof(ImageOptionalHeader32) == 244 &&
	"Optional Header 32 Must be 244 bytes");

#pragma pack(pop)

using ImageOptionalHeader =
std::variant<ImageOptionalHeader32, ImageOptionalHeader64>;

struct ImageNTHeaders {
	std::array<std::byte, 4> Signature = SIGNATURE;
	ImageFileHeader FileHeader;
	ImageOptionalHeader OptionalHeader;
};

static_assert(sizeof(ImageNTHeaders) == 272 &&
	"ImageNTHeaders must be 272 bytes with padding");

enum class ImageSectionFlags : uint32_t {
	// section should not be padded to next boundary
	ScnTypeNoPad = 0x00000008,

	// the section contains executable code (.text)
	ScnCntCode = 0x00000020,

	// the section contains initialized data (.data)
	ScnCntInitializedData = 0x00000040,

	// the section contains uninitialized data (.bss)
	ScnCntUnInitializedData = 0x00000080,

	// reserved for future use
	ScnLinkOther = 0x00000100,
	// contains comments or other information.
	ScnLnkInfo = 0x00000200,

	// the section will not become part of the image. this is valid only for
	// object files
	ScnLnkRemove = 0x00000800,

	// contains COMDAT data
	ScnLnkComDat = 0x00001000,

	// The section contains data referenced through the global pointer (GP)
	ScnGpRel = 0x00008000,

	// reserved for future use
	ScnMemPurgeable = 0x00020000,
	// reserved for future use
	ScnMem16Bit = 0x00020000,
	// reserved for future use
	ScnMemLocked = 0x00040000,
	// reserved for future use
	ScnMemPreload = 0x00080000,

	// Align data on a 1-byte boundary. Valid only for object files.
	ScnMemAlign1Bytes = 0x00100000,
	ScnMemAlign2Bytes = 0x00200000,
	ScnMemAlign4Bytes = 0x00300000,
	// Align data on a 8-byte boundary. Valid only for object files.
	ScnMemAlign8Bytes = 0x00400000,

	ScnMemAlign16Bytes = 0x00500000,
	ScnMemAlign32Bytes = 0x00600000,
	ScnMemAlign64Bytes = 0x00700000,

	// Align data on a 128-byte boundary. Valid only for object files.
	ScnMemAlign128Bytes = 0x00800000,
	ScnMemAlign256Bytes = 0x00900000,
	ScnMemAlign512Bytes = 0x00A00000,

	ScnMemAlign1024Bytes = 0x00B00000,
	ScnMemAlign2048Bytes = 0x00C00000,
	ScnMemAlign4096Bytes = 0x00D00000,
	ScnMemAlign8192Bytes = 0x00E00000,

	// the section contains extended relocations.
	ScnNRelocOvfl = 0x01000000,

	// the section can be discarded as needed
	ScnMemDiscardable = 0x02000000,
	// the section cannot be cached
	ScnMemNotCached = 0x04000000,
	// the section is not pageable
	ScnMemNotPaged = 0x08000000,

	// the section can be in shared memory
	ScnMemShared = 0x10000000,
	// the section can be executed as code
	ScnMemExecute = 0x20000000,

	// the section can be read
	ScnMemRead = 0x40000000,
	// the section can be written to
	ScnMemWrite = 0x80000000

};

struct ImageSectionHeader {
	// An 8-byte, null-padded UTF-8 encoded string, if the string is exactly 8
	// characters long, then there is no null terminator
	std::array<char, 8> Name;

	// the total size of the section when loaded into memory
	uint32_t VirtualSize;

	// For executable images, the address of the first byte of the section
	// relative to the image base when the section is loaded into memory. For
	// object files, this field is the address of the first byte before relocation
	// is applied; for simplicity, compilers should set this to zero
	uint32_t VirtualAddress;

	// The size of the section (for object files) or the size of the initialized
	// data on disk (for image files). For executable images, this must be a
	// multiple of FileAlignment from the optional header
	uint32_t SizeOfRawData;

	//  The file pointer to the first page of the section within the COFF file.
	//  For executable images, this must be a multiple of FileAlignment from the
	//  optional header. For object files, the value should be aligned on a 4-byte
	//  boundary for best performance.
	uint32_t PointerToRawData;

	uint32_t PointerToRelocations;
	uint32_t PointerToLinenumbers;

	uint16_t NumberOfRelocations;
	uint16_t NumberOfLinenumbers;

	// The flags that describe the characteristics of the section
	ImageSectionFlags Characteristics;
};

static_assert(sizeof(ImageSectionHeader) == 40 &&
	"Section header must be 40 bytes");

struct ImageImportDescriptor {
	// RVA of Import Lookup Table (ILT/INT)
	// contains name or ordinal for each import.
	uint32_t OriginalFirstThunk;
	// the time date stamp is set to zero until image is bound.  After the image
	// is bound, this field is set to the time/data stamp of the DLL
	uint32_t TimeDateStamp;
	// the index of the first forward reference
	uint32_t ForwardChain;
	// the address of ASCII string that contains the name of the DLL
	uint32_t Name;
	// RVA of the import Address Table (IAT)
	// The contents of this table are identical to the contents of the import
	// lookup table until the image is bound.
	uint32_t FirstThunk;
};

static_assert(sizeof(ImageImportDescriptor) == 20);

// =================utilities===============================

constexpr std::optional<uint32_t>
RvaToFileOffset(uint32_t Rva, const ImageSectionHeader& Section) noexcept {
	if (Rva >= Section.VirtualAddress &&
		Rva < Section.VirtualAddress + Section.SizeOfRawData) {
		return std::uint32_t{ Rva - Section.VirtualAddress +
							 Section.PointerToRawData };
	}

	return std::nullopt;
}

// todo: put somewhere else
constexpr std::string_view MachineToStringView(MachineType Type) noexcept {
	switch (Type) {
	case MachineType::MACHINE_UNKNOWN:
		return "Unknown";
	case MachineType::MACHINE_ALPHA:
		return "Alpha";
	case MachineType::MACHINE_ALPHA64:
		return "Alpha64";
	case MachineType::MACHINE_AMD64:
		return "AMD64";
	case MachineType::MACHINE_ARM:
		return "ARM";
	case MachineType::MACHINE_ARM64:
		return "ARM64";
	case MachineType::MACHINE_I386:
		return "I386";
	case MachineType::MACHINE_RISCV32:
		return "RISC-V 32";
	case MachineType::MACHINE_RISCV64:
		return "RISC-V 64";
	case MachineType::MACHINE_RISCV128:
		return "RISC-V 128";
	case MachineType::MACHINE_POWERPC:
		return "PowerPC";

	default:
		std::unreachable();
	}
}

constexpr std::string_view PEFormatToStringView(PEFormat Format) noexcept {
	switch (Format) {
	case PEFormat::PE32:
		return "PE32";
	case PEFormat::PE64:
		return "PE32+";

	default:
		std::unreachable();
	};
}

constexpr std::string_view
CharacteristicsToStringView(FileCharacteristics Flag) noexcept {
	switch (Flag) {
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

inline std::string CharacteristicsToString(std::uint16_t Characteristics) {
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

	for (const auto Flag : Flags) {
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

constexpr std::string_view
ImageSubSystemToStringView(ImageSubsystemType Type) noexcept {
	switch (Type) {
	case ImageSubsystemType::Unknown:
		return "Unknown";

	case ImageSubsystemType::Native:
		return "Native";

	case ImageSubsystemType::GUI:
		return "Windows GUI";

	case ImageSubsystemType::WindowsCUI:
		return "Windows CUI";

	case ImageSubsystemType::OS2CUI:
		return "OS/2 CUI";

	case ImageSubsystemType::PosixCUI:
		return "POSIX CUI";

	case ImageSubsystemType::NativeWindows:
		return "Native Win9x";

	case ImageSubsystemType::WindowsCeGUI:
		return "Windows CE GUI";

	case ImageSubsystemType::EFIApplication:
		return "EFI Application";

	case ImageSubsystemType::EFIBootServiceDriver:
		return "EFI Boot Service Driver";

	case ImageSubsystemType::EFIRuntimeDriver:
		return "EFI Runtime Driver";

	case ImageSubsystemType::EFIRom:
		return "EFI ROM Image";

	case ImageSubsystemType::XBOX:
		return "XBOX";

	case ImageSubsystemType::WindowsBootApplication:
		return "Windows Boot Application";

	default:
		std::unreachable();
	}
}

inline std::string ImageSectionFlagsToString(uint32_t Flags) {
	std::string Result;

	auto Append = [&](std::string_view Name) {
		if (!Result.empty())
			Result += " | ";

		Result += Name;
		};

	constexpr struct {
		uint32_t Value{};
		std::string_view Name{};
	} FlagsTable[] = {
		{ 0x00000008, "TYPE_NO_PAD" },
		{ 0x00000020, "CNT_CODE" },
		{ 0x00000040, "CNT_INITIALIZED_DATA" },
		{ 0x00000080, "CNT_UNINITIALIZED_DATA" },
		{ 0x00000100, "LNK_OTHER" },
		{ 0x00000200, "LNK_INFO" },
		{ 0x00000800, "LNK_REMOVE" },
		{ 0x00001000, "LNK_COMDAT" },
		{ 0x00008000, "GP_REL" },
		{ 0x00020000, "MEM_PURGEABLE" },
		{ 0x00020000, "MEM_16BIT" },
		{ 0x00040000, "MEM_LOCKED" },
		{ 0x00080000, "MEM_PRELOAD" },
		{ 0x01000000, "NRELOC_OVFL" },
		{ 0x02000000, "MEM_DISCARDABLE" },
		{ 0x04000000, "MEM_NOT_CACHED" },
		{ 0x08000000, "MEM_NOT_PAGED" },
		{ 0x10000000, "MEM_SHARED" },
		{ 0x20000000, "MEM_EXECUTE" },
		{ 0x40000000, "MEM_READ" },
		{ 0x80000000, "MEM_WRITE" },
	};

	for (auto [Value, Name] : FlagsTable) {
		if ((Flags & Value) == Value)
			Append(Name);
	}

	return Result;
}