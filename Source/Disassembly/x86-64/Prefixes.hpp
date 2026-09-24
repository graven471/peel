#pragma once

#include <array>
#include <ranges>

/*
 15 bytes
 [instruction prefixes] [opcode] [optional ModR/M] [optional SIB]
 [optional displacement] [optional immediate]

 but each instruction can have less like 90 is 1-byte ix NOP
 so don't assume

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

constexpr std::array RexPrefix = {std::byte{0x40}, std::byte{0x41}, std::byte{0x42}, std::byte{0x43},
                                  std::byte{0x44}, std::byte{0x45}, std::byte{0x46}, std::byte{0x47},
                                  std::byte{0x48}, std::byte{0x49}, std::byte{0x4A}, std::byte{0x4B},
                                  std::byte{0x4C}, std::byte{0x4D}, std::byte{0x4E}, std::byte{0x4F}};

constexpr std::array LegacyGroup1Prefixes = {std::byte{0xF0}, std::byte{0xF2}, std::byte{0xF3}};

constexpr std::array LegacyGroup2Prefixes = {std::byte{0x2E}, std::byte{0x36}, std::byte{0x3E},
                                             std::byte{0x26}, std::byte{0x64}, std::byte{0x65}};

constexpr std::byte LegacyGroup3Prefix = std::byte{0x66};
constexpr std::byte LegacyGroup4Prefix = std::byte{0x67};

constexpr bool IsPrefix(const std::byte byte) noexcept
{
  return std::ranges::contains(LegacyGroup1Prefixes, byte) || std::ranges::contains(LegacyGroup2Prefixes, byte)
         || byte == LegacyGroup3Prefix || byte == LegacyGroup4Prefix || std::ranges::contains(RexPrefix, byte);
}

// test data
constexpr std::array<std::byte, 15> TestBytes{
    std::byte{0x48},  // REX
    std::byte{0x64},  // Group 2
    std::byte{0x8B},  // stop
    std::byte{0x0F},  // shouldn't be reached by prefix scanner
    std::byte{0x66}, std::byte{0x67}, std::byte{0x4C}, std::byte{0x90}, std::byte{0x00}, std::byte{0x00},
    std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
};

// 1-byte opcode: NOP (0x90)
constexpr std::array<std::byte, 5> Test1ByteOpcode{
    std::byte{0x48}, std::byte{0x90},                   // opcode = 0x90 (NOP)
    std::byte{0x00}, std::byte{0x00}, std::byte{0x00},  // padding
};

// map1: MOVSX r64, r/m16 (0F BF)
constexpr std::array<std::byte, 6> TestMap1Opcode{
    std::byte{0x48}, std::byte{0x66}, std::byte{0x0F},  // escape
    std::byte{0xBF},                                    // opcode
    std::byte{0x00}, std::byte{0x00},                   // padding
};

constexpr std::array<std::byte, 4> TestMovsx{
    std::byte{0x48},  // REX.W (64-bit destination)
    std::byte{0x0F},  // escape
    std::byte{0xBF},  // opcode MOVSX
    std::byte{0xC3},  // ModR/M: mod=11 reg=000 rm=011 -> rax, rbx
};

constexpr std::array<std::byte, 5> TestMap3Opcode{
    std::byte{0x65}, std::byte{0x0F},  // escape
    std::byte{0x3A},                   // map3 select
    std::byte{0x08},                   // opcode
    std::byte{0x00},                   // padding
};

// ADD Ev, Gv
constexpr std::array<std::byte, 2> TestModRMOpcode{std::byte{0x8B}, std::byte{0xD8}};

inline auto TestBytesSpan = std::span{TestModRMOpcode};