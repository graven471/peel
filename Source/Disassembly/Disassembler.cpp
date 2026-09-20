#include "Disassembler.hpp"

#include <algorithm>
#include <print>
#include <ranges>
#include <span>

void Disassembler::DoIt() {
  constexpr std::array TextName{'.', 't', 'e', 'x', 't', '\0', '\0', '\0'};

  auto It = std::ranges::find_if(
      Image->SectionHeaders, [&](const ImageSection& Section) {
        return std::ranges::equal(Section.Header.Name, TextName);
      });

  if (It == Image->SectionHeaders.end()) {
    std::println(stderr, "no .text section is present");
    return;
  }

  std::println("found text section");

  ImageSection& TextSection = *It;

  // 15 bytes
  //[instruction prefixes] [opcode] [optional ModR/M] [optional SIB]
  //[optional displacement] [optional immediate]

  // but each instruction can have less like 90 is 1-byte ix NOP
  // so don't assume

  /*
    AMD/INTEL 64 REX Prefix only for PE32+ images and note Not all instruction
    require a REX prefix in 64-bit mode.

   in AMD64 mode 15 bytes:
   [legacy prefixes]        max 4 bytes
   [optional REX Prefix]    max 1 byte
   [opcode]                 1, 2 or 3 bytes
   [optional ModR/M]        1 byte
   [optional SIB]           1 byte
   [optional displacement]  upto 4 bytes
   [optional immediate]     upto 8 bytes

   REX prefix is between 0x40-0x4F
  */

  constexpr std::array RexPrefix = {
      std::byte{0x40}, std::byte{0x41}, std::byte{0x42}, std::byte{0x43},
      std::byte{0x44}, std::byte{0x45}, std::byte{0x46}, std::byte{0x47},
      std::byte{0x48}, std::byte{0x49}, std::byte{0x4A}, std::byte{0x4B},
      std::byte{0x4C}, std::byte{0x4D}, std::byte{0x4E}, std::byte{0x4F}};

  constexpr std::array LegacyGroup1Prefixes = {std::byte{0xF0}, std::byte{0xF2},
                                               std::byte{0xF3}};

  constexpr std::array LegacyGroup2Prefixes = {
      std::byte{0x2E}, std::byte{0x36}, std::byte{0x3E},
      std::byte{0x26}, std::byte{0x64}, std::byte{0x65}};

  constexpr std::byte LegacyGroup3Prefix = std::byte{0x66};
  constexpr std::byte LegacyGroup4Prefix = std::byte{0x67};

  std::span<const std::byte> InstructionEncoding =
      TextSection.Data.subspan(0, 32);

  auto ContainsGroup1Prefix = [&](const std::byte Prefix) {
    return std::ranges::any_of(LegacyGroup1Prefixes,
                               [&](const auto& P) { return Prefix == P; });
  };

  auto ContainsGroup2Prefix = [&](const std::byte Prefix) {
    return std::ranges::any_of(LegacyGroup2Prefixes,
                               [&](const auto& P) { return Prefix == P; });
  };

  auto ContainsRexPrefix = [&](const std::byte Prefix) -> bool {
    return std::ranges::any_of(
        RexPrefix, [&](const std::byte& Rex) { return Rex == Prefix; });
  };

  // for now keep it small
  for (int i = 0; i < 32; ++i) {
    // skip padding well 0xcc is also INT3 instruction
    if (InstructionEncoding[i] == std::byte{0xcc}) continue;

    if (ContainsRexPrefix(InstructionEncoding[i])) {
      std::print("Rex prefix: 0x{:02X} ",
                 std::to_integer<uint8_t>(InstructionEncoding[i]));
    }
  }

  std::println();
}

void Disassembler::PrefixScanner(
    std::span<const std::byte> InstructionEncoding) {
  // for I in instruction encoding:
  //   is I in legacy group 1-4 prefix ?
  //   is I in REX prefix ?
  //   if I is prefix then push I to somewhere (yet to be defined)
  //   go to next byte I + 1 check if its in prefix
  //   repeat until we found something that is not prefix
  //   if not stop and return subspan from there
  //   [2E 66 3E 48 8B 05 E7 33 03]
  //    ----------- |
  //    prefix      new subspan loc
  // slower but this is first impl can improve performance and speed later
}
