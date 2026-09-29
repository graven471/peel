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

[[nodiscard]] static PeelResult<std::span<const std::byte>> map_file(const std::string& file_name)
{
  HANDLE file_handle = CreateFileA(file_name.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

  if(file_handle == INVALID_HANDLE_VALUE)
  {
    return std::unexpected(MakeWin32Error(::GetLastError()));
  }

  LARGE_INTEGER file_size;

  if(GetFileSizeEx(file_handle, &file_size) == 0)
  {
    return std::unexpected(MakeWin32Error(::GetLastError()));
  }

  HANDLE file_mapping = CreateFileMapping(file_handle, NULL, PAGE_READONLY, 0, 0, NULL);

  if(file_mapping == NULL)
  {
    CloseHandle(file_handle);
    return std::unexpected(MakeWin32Error(::GetLastError()));
  }

  CloseHandle(file_handle);

  LPVOID base_address = MapViewOfFile(file_mapping, FILE_MAP_READ, 0, 0, 0);

  if(base_address == NULL) [[unlikely]]
  {
    return std::unexpected(MakeWin32Error(::GetLastError()));
  }

  return std::span<const std::byte>{static_cast<const std::byte*>(base_address), static_cast<std::size_t>(file_size.QuadPart)};
}

int main(int argc, const char* argv[])
{
  //if(argc < 2)
  //{
  //  std::println(stderr, "Usage: peel <file.exe>");
  //  return EXIT_FAILURE;
  //}

  //std::string file_name = argv[1];

  //if(!file_name.ends_with("exe"))
  //{
  //  std::println(stderr, "invalid format");
  //  return EXIT_FAILURE;
  //}

  //std::println("filename: {}", file_name);

  std::string file_name = "C:\\Windows\\System32\\MRT.exe";

  PeelResult<std::span<const std::byte>> mapped_file = map_file(file_name);

  if(!mapped_file)
  {
    std::println(stderr, "\033[31mError: \033[0m {}", mapped_file.error().ToString());

    return EXIT_FAILURE;
  }

  PEParser parser{*mapped_file};

  auto image = parser.parse();

  if(!image)
  {
    std::println(stderr, "\033[31mError: \033[0m {}", image.error().ToString());

    return EXIT_FAILURE;
  }

  Disassembler disassembler{&*image};

  //for(std::size_t i = 0; i < 10'000; ++i)
  //{
  disassembler.disassemble();
  //}

  return EXIT_SUCCESS;
}