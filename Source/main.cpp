// Peel is a PE File Visualizer

#define UNICODE
#define _UNICODE

// Prevent the definition of min and max macros which conflict with C++ standard
// libraries
#define NOMINMAX

// Exclude rarely-used features and APIs from the Windows headers to speed up
// compilation
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <cstddef>
#include <print>
#include <span>
#include <string>

#include "Core/Error.hpp"
#include "Disassembly/Disassembler.hpp"
#include "Parser/Parser.hpp"

[[nodiscard]] static PeelResult<std::span<const std::byte>> MapFile(
    const std::string& InFileName) {
  // use A prefix for now
  HANDLE HFile = CreateFileA(InFileName.c_str(), GENERIC_READ, NULL, NULL,
                             OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

  if (HFile == INVALID_HANDLE_VALUE) {
    return std::unexpected(MakeWin32Error(::GetLastError()));
  }

  // get the file size
  LARGE_INTEGER FileSize;

  if (GetFileSizeEx(HFile, &FileSize) == 0) {
    return std::unexpected(MakeWin32Error(::GetLastError()));
  }

  // create file mapping aka describing backing store
  HANDLE HMapping = CreateFileMapping(HFile, NULL, PAGE_READONLY, 0, 0, NULL);

  if (HMapping == NULL) {
    CloseHandle(HFile);
    return std::unexpected(MakeWin32Error(::GetLastError()));
  }

  // CreateFileMapping keeps the mapping alive internally
  CloseHandle(HFile);

  // map the file contents into this process virtual address space
  LPVOID BasePtr = MapViewOfFile(HMapping, FILE_MAP_READ, 0, 0, 0);

  if (BasePtr == NULL) {
    return std::unexpected(MakeWin32Error(::GetLastError()));
  }

  return std::span<const std::byte>{
      static_cast<const std::byte*>(BasePtr),
      static_cast<std::size_t>(FileSize.QuadPart)};
}

int main(int argc, const char* argv[]) {
  if (argc < 2) {
    std::println(stderr, "Usage: peel <file.exe>");
    return EXIT_FAILURE;
  }

  // i can use std::string_view here but its not guaranteed to be null
  // terminated and since most of the time file name will be < 15 chars
  // internally it can use SSO to avoid heap
  std::string FileName = argv[1];

  if (!FileName.ends_with("exe")) {
    std::println(stderr, "PE format is required");
    return EXIT_FAILURE;
  }

  std::println("filename: {}", FileName);

  PeelResult<std::span<const std::byte>> Mapping = MapFile(FileName);

  if (!Mapping) {
    std::println(stderr, "\033[31mError: \033[0m {}",
                 Mapping.error().ToString());
    return EXIT_FAILURE;
  }

  PEParser Parser{*Mapping};

  PeelResult<PEImage> Image = Parser.Parse();

  if (!Image) {
    std::println(stderr, "\033[31mError: \033[0m {}", Image.error().ToString());
    return EXIT_FAILURE;
  }

  Disassembler Diasas{&*Image};

  Diasas.DoIt();

  return EXIT_SUCCESS;
}
