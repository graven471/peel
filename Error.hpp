#pragma once

#include <print>
#include <string_view>

#include <source_location>

#include <windows.h>

#include <expected>

enum class PeelErrorCode : uint32_t {
	PEEL_UNKNOWN,
	PEEL_SUCCESS,

	PEEL_ACCESS_DENIED,
	PEEL_FILE_NOT_FOUND,
	PEEL_PATH_NOT_FOUND,
	PEEL_INVALID_HANDLE,
	PEEL_INVALID_PARAMETER,
	PEEL_OUT_OF_MEMORY,
	PEEL_ALREADY_EXISTS,
	PEEL_BUSY,
	PEEL_TIMEOUT,
	PEEL_CANCELLED,
	PEEL_NOT_SUPPORTED,
	PEEL_IO_ERROR,

	PEEL_INVALID_IMAGE,
	PEEL_INVALID_DOS_HEADER,
	PEEL_INVALID_NT_HEADER,
	PEEL_INVALID_FILE_HEADER,
	PEEL_INVALID_OPTIONAL_HEADER,

	PEEL_INVALID_SIGNATURE,
	PEEL_INVALID_MAGIC,

	PEEL_TRUNCATED_FILE,
	PEEL_INVALID_FILE_SIZE,

	PEEL_INVALID_SECTION,
	PEEL_INVALID_SECTION_TABLE,
	PEEL_INVALID_SECTION_ALIGNMENT,

	PEEL_INVALID_RVA,
	PEEL_INVALID_FILE_OFFSET,
	PEEL_INVALID_VIRTUAL_ADDRESS,

	PEEL_INVALID_DATA_DIRECTORY,
	PEEL_INVALID_DATA_DIRECTORY_SIZE,

	PEEL_INVALID_IMPORT_DIRECTORY,
	PEEL_INVALID_EXPORT_DIRECTORY,
	PEEL_INVALID_RELOCATION_DIRECTORY,
	PEEL_INVALID_RESOURCE_DIRECTORY,
	PEEL_INVALID_EXCEPTION_DIRECTORY,
	PEEL_INVALID_DEBUG_DIRECTORY,
	PEEL_INVALID_TLS_DIRECTORY,

	PEEL_INVALID_IMPORT_DESCRIPTOR,
	PEEL_INVALID_EXPORT_DIRECTORY_ENTRY,

	PEEL_INVALID_RELOCATION_BLOCK,
	PEEL_INVALID_RELOCATION_ENTRY,

	PEEL_INVALID_RESOURCE_DIRECTORY_ENTRY,

	PEEL_INVALID_STRING,
	PEEL_INVALID_STRING_TABLE,
	PEEL_INVALID_SYMBOL_TABLE,

	PEEL_INVALID_ENTRY_POINT,

	PEEL_UNSUPPORTED_MACHINE,
	PEEL_UNSUPPORTED_SUBSYSTEM,
	PEEL_UNSUPPORTED_PE_VERSION,
	PEEL_UNSUPPORTED_SECTION,

	PEEL_OUT_OF_BOUNDS,
	PEEL_OVERFLOW,
	PEEL_INVALID_STATE,
};

struct PeelError {
	PeelErrorCode Code = PeelErrorCode::PEEL_UNKNOWN;
	std::source_location Location = std::source_location::current();
	DWORD Win32Code;

	constexpr PeelError(PeelErrorCode InCode, DWORD InWin32Code = ::GetLastError(), std::source_location InLocation = std::source_location::current())
		noexcept : Code(InCode), Win32Code(InWin32Code), Location(InLocation) {
	}

	[[nodiscard]] std::string ToString() const {
		return std::format(
			"PeelError {{\n"
			"    code:     {}\n"
			"    win32:    {} (0x{:08X})\n"
			"    location: {}:{}:{}\n"
			"    function: {}\n"
			"}}",
			CodeToString(),
			Win32Code,
			Win32Code,
			Location.file_name(),
			Location.line(),
			Location.column(),
			Location.function_name()
		);
	}

private:
	constexpr std::string_view CodeToString() const noexcept {
		switch (Code) {
		case PeelErrorCode::PEEL_UNKNOWN:
			return "UNKNOWN";

		case PeelErrorCode::PEEL_SUCCESS:
			return "SUCCESS";

		case PeelErrorCode::PEEL_ACCESS_DENIED:
			return "ACCESS_DENIED";

		case PeelErrorCode::PEEL_FILE_NOT_FOUND:
			return "FILE_NOT_FOUND";

		case PeelErrorCode::PEEL_PATH_NOT_FOUND:
			return "PATH_NOT_FOUND";

		case PeelErrorCode::PEEL_INVALID_HANDLE:
			return "INVALID_HANDLE";

		case PeelErrorCode::PEEL_INVALID_PARAMETER:
			return "INVALID_PARAMETER";

		case PeelErrorCode::PEEL_OUT_OF_MEMORY:
			return "OUT_OF_MEMORY";

		case PeelErrorCode::PEEL_ALREADY_EXISTS:
			return "ALREADY_EXISTS";

		case PeelErrorCode::PEEL_BUSY:
			return "BUSY";

		case PeelErrorCode::PEEL_TIMEOUT:
			return "TIMEOUT";

		case PeelErrorCode::PEEL_CANCELLED:
			return "CANCELLED";

		case PeelErrorCode::PEEL_NOT_SUPPORTED:
			return "NOT_SUPPORTED";

		case PeelErrorCode::PEEL_IO_ERROR:
			return "IO_ERROR";

		case PeelErrorCode::PEEL_INVALID_IMAGE:
			return "INVALID_IMAGE";

		case PeelErrorCode::PEEL_INVALID_DOS_HEADER:
			return "INVALID_DOS_HEADER";

		case PeelErrorCode::PEEL_INVALID_NT_HEADER:
			return "INVALID_NT_HEADER";

		case PeelErrorCode::PEEL_INVALID_FILE_HEADER:
			return "INVALID_FILE_HEADER";

		case PeelErrorCode::PEEL_INVALID_OPTIONAL_HEADER:
			return "INVALID_OPTIONAL_HEADER";

		case PeelErrorCode::PEEL_INVALID_SIGNATURE:
			return "INVALID_SIGNATURE";

		case PeelErrorCode::PEEL_INVALID_MAGIC:
			return "INVALID_MAGIC";

		case PeelErrorCode::PEEL_TRUNCATED_FILE:
			return "TRUNCATED_FILE";

		case PeelErrorCode::PEEL_INVALID_FILE_SIZE:
			return "INVALID_FILE_SIZE";

		case PeelErrorCode::PEEL_INVALID_SECTION:
			return "INVALID_SECTION";

		case PeelErrorCode::PEEL_INVALID_SECTION_TABLE:
			return "INVALID_SECTION_TABLE";

		case PeelErrorCode::PEEL_INVALID_SECTION_ALIGNMENT:
			return "INVALID_SECTION_ALIGNMENT";

		case PeelErrorCode::PEEL_INVALID_RVA:
			return "INVALID_RVA";

		case PeelErrorCode::PEEL_INVALID_FILE_OFFSET:
			return "INVALID_FILE_OFFSET";

		case PeelErrorCode::PEEL_INVALID_VIRTUAL_ADDRESS:
			return "INVALID_VIRTUAL_ADDRESS";

		case PeelErrorCode::PEEL_INVALID_DATA_DIRECTORY:
			return "INVALID_DATA_DIRECTORY";

		case PeelErrorCode::PEEL_INVALID_DATA_DIRECTORY_SIZE:
			return "INVALID_DATA_DIRECTORY_SIZE";

		case PeelErrorCode::PEEL_INVALID_IMPORT_DIRECTORY:
			return "INVALID_IMPORT_DIRECTORY";

		case PeelErrorCode::PEEL_INVALID_EXPORT_DIRECTORY:
			return "INVALID_EXPORT_DIRECTORY";

		case PeelErrorCode::PEEL_INVALID_RELOCATION_DIRECTORY:
			return "INVALID_RELOCATION_DIRECTORY";

		case PeelErrorCode::PEEL_INVALID_RESOURCE_DIRECTORY:
			return "INVALID_RESOURCE_DIRECTORY";

		case PeelErrorCode::PEEL_INVALID_EXCEPTION_DIRECTORY:
			return "INVALID_EXCEPTION_DIRECTORY";

		case PeelErrorCode::PEEL_INVALID_DEBUG_DIRECTORY:
			return "INVALID_DEBUG_DIRECTORY";

		case PeelErrorCode::PEEL_INVALID_TLS_DIRECTORY:
			return "INVALID_TLS_DIRECTORY";

		case PeelErrorCode::PEEL_INVALID_IMPORT_DESCRIPTOR:
			return "INVALID_IMPORT_DESCRIPTOR";

		case PeelErrorCode::PEEL_INVALID_EXPORT_DIRECTORY_ENTRY:
			return "INVALID_EXPORT_DIRECTORY_ENTRY";

		case PeelErrorCode::PEEL_INVALID_RELOCATION_BLOCK:
			return "INVALID_RELOCATION_BLOCK";

		case PeelErrorCode::PEEL_INVALID_RELOCATION_ENTRY:
			return "INVALID_RELOCATION_ENTRY";

		case PeelErrorCode::PEEL_INVALID_RESOURCE_DIRECTORY_ENTRY:
			return "INVALID_RESOURCE_DIRECTORY_ENTRY";

		case PeelErrorCode::PEEL_INVALID_STRING:
			return "INVALID_STRING";

		case PeelErrorCode::PEEL_INVALID_STRING_TABLE:
			return "INVALID_STRING_TABLE";

		case PeelErrorCode::PEEL_INVALID_SYMBOL_TABLE:
			return "INVALID_SYMBOL_TABLE";

		case PeelErrorCode::PEEL_INVALID_ENTRY_POINT:
			return "INVALID_ENTRY_POINT";

		case PeelErrorCode::PEEL_UNSUPPORTED_MACHINE:
			return "UNSUPPORTED_MACHINE";

		case PeelErrorCode::PEEL_UNSUPPORTED_SUBSYSTEM:
			return "UNSUPPORTED_SUBSYSTEM";

		case PeelErrorCode::PEEL_UNSUPPORTED_PE_VERSION:
			return "UNSUPPORTED_PE_VERSION";

		case PeelErrorCode::PEEL_UNSUPPORTED_SECTION:
			return "UNSUPPORTED_SECTION";

		case PeelErrorCode::PEEL_OUT_OF_BOUNDS:
			return "OUT_OF_BOUNDS";

		case PeelErrorCode::PEEL_OVERFLOW:
			return "OVERFLOW";

		case PeelErrorCode::PEEL_INVALID_STATE:
			return "INVALID_STATE";
		}

		return "UNKNOWN";
	}
};

constexpr PeelErrorCode TranslateToPeelError(DWORD ErrorCode = ::GetLastError()) noexcept {
	switch (ErrorCode) {
	case ERROR_SUCCESS:
		return PeelErrorCode::PEEL_SUCCESS;

	case ERROR_ACCESS_DENIED:
	case ERROR_PRIVILEGE_NOT_HELD:
		return PeelErrorCode::PEEL_ACCESS_DENIED;

	case ERROR_FILE_NOT_FOUND:
		return PeelErrorCode::PEEL_FILE_NOT_FOUND;

	case ERROR_PATH_NOT_FOUND:
		return PeelErrorCode::PEEL_PATH_NOT_FOUND;

	case ERROR_INVALID_HANDLE:
		return PeelErrorCode::PEEL_INVALID_HANDLE;

	case ERROR_INVALID_PARAMETER:
		return PeelErrorCode::PEEL_INVALID_PARAMETER;

	case ERROR_NOT_ENOUGH_MEMORY:
	case ERROR_OUTOFMEMORY:
		return PeelErrorCode::PEEL_OUT_OF_MEMORY;

	case ERROR_ALREADY_EXISTS:
	case ERROR_FILE_EXISTS:
		return PeelErrorCode::PEEL_ALREADY_EXISTS;

	case ERROR_BUSY:
		return PeelErrorCode::PEEL_BUSY;

	case ERROR_TIMEOUT:
		return PeelErrorCode::PEEL_TIMEOUT;

	case ERROR_OPERATION_ABORTED:
		return PeelErrorCode::PEEL_CANCELLED;

	case ERROR_NOT_SUPPORTED:
	case ERROR_CALL_NOT_IMPLEMENTED:
		return PeelErrorCode::PEEL_NOT_SUPPORTED;

	case ERROR_HANDLE_EOF:
	case ERROR_READ_FAULT:
	case ERROR_WRITE_FAULT:
	case ERROR_CRC:
	case ERROR_IO_DEVICE:
		return PeelErrorCode::PEEL_IO_ERROR;

	default:
		return PeelErrorCode::PEEL_UNKNOWN;
	}
}

[[nodiscard]] PeelError MakePeelError(DWORD ErrorCode, std::source_location Location = std::source_location::current()) noexcept {
	return PeelError{
		TranslateToPeelError(ErrorCode),
		ErrorCode,
		Location
	};
}

template <typename T>
using PeelResult = std::expected<T, PeelError>;