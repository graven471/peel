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
  // use A prefix for now
  HANDLE file_handle = CreateFileA(file_name.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

  if(file_handle == INVALID_HANDLE_VALUE)
  {
    return std::unexpected(MakeWin32Error(::GetLastError()));
  }

  // get the file size
  LARGE_INTEGER file_size;

  if(GetFileSizeEx(file_handle, &file_size) == 0)
  {
    return std::unexpected(MakeWin32Error(::GetLastError()));
  }

  // create file mapping aka describing backing store
  HANDLE file_mapping = CreateFileMapping(file_handle, NULL, PAGE_READONLY, 0, 0, NULL);

  if(file_mapping == NULL)
  {
    CloseHandle(file_handle);
    return std::unexpected(MakeWin32Error(::GetLastError()));
  }

  // CreateFileMapping keeps the mapping alive internally
  CloseHandle(file_handle);

  // map the file contents into this process virtual address space
  LPVOID base_ptr = MapViewOfFile(file_mapping, FILE_MAP_READ, 0, 0, 0);

  if(base_ptr == NULL)
  {
    return std::unexpected(MakeWin32Error(::GetLastError()));
  }

  return std::span<const std::byte>{static_cast<const std::byte*>(base_ptr), static_cast<std::size_t>(file_size.QuadPart)};
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

  PeelResult<std::span<const std::byte>> mapping = map_file(file_name);

  if(!mapping)
  {
    std::println(stderr, "\033[31mError: \033[0m {}", mapping.error().ToString());

    return EXIT_FAILURE;
  }

  PEParser parser{*mapping};

  PeelResult<PEImage> image = parser.Parse();

  if(!image)
  {
    std::println(stderr, "\033[31mError: \033[0m {}", image.error().ToString());

    return EXIT_FAILURE;
  }

  Disassembler disassembler{&*image};

  //for(std::size_t i = 0; i < 10'000; ++i)
  //{
  disassembler.do_it();
  //}

  return EXIT_SUCCESS;
}