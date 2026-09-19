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
    instruction prefix:
      instruction prefix is divided into four groups, each with a set of
    allowable prefix code Group 1 through 4 may be placed in any order relative
    to each other

    GROUP 1: LOCK
    0xF0 (1111 0000) = LOCK
    0xF2 (1111 0010) = REPNE/REPNZ
    0xF3 (1111 0011) = REP or REPE/REPZ

    0XF2 can also also represent BND prefix (intel MPX) ignore that

    GROUP 2: Segment override prefixes
    0x2E - CS segment override (use with any branch instruction is reversed)

    0x36 - SS segment override prefix (use with any branch instruction is
    reversed)

    0x3E - DS segment override prefix (use with any branch
    instruction is reversed)

    0x26 - ES segment override prefix (use with any branch
    instruction is reversed)

    0x64 - FS segment override prefix (use with any branch
    instruction is reversed)

    0x65 - GS segment override prefix (use with any branch
    instruction is reversed)

    Branch hints:
    - 0x2E - Branch not taken (use only with `Jcc` instructions.)
    - 0x3E - Branch taken (use only with `Jcc` instructions.)

    GROUP 3:
    - operand size override prefix is encoded using 0x66 (0x66 is also used as
    mandatory prefix for some instructions).
    use a operand size than the instruction will normally use like in 32
    bit-mode operand is normally 32-bit it can change operand to 16 bit

    GROUP 4:
     - 0x67 - Address-size override prefix
     use a different address size when calculating memory addresses
     normal -> 64-bit addressing
     67     -> 32-bit addressing

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
