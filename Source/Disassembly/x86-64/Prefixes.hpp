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

enum class Prefixtype : std::uint8_t
{
  GROUP1,
  GROUP2,
  GROUP3,
  GROUP4,
  REX,
  NONE
};

struct PrefixMetadata
{
  Prefixtype type{Prefixtype::NONE};
  bool       is_prefix{false};
};

static consteval std::array<PrefixMetadata, 256> build_prefix_table() noexcept
{
  std::array<PrefixMetadata, 256> table{};

  for(auto b : LegacyGroup1Prefixes)
  {
    table[std::to_integer<std::uint8_t>(b)] = {.type = Prefixtype::GROUP1, .is_prefix = true};
  }

  for(auto b : LegacyGroup2Prefixes)
  {
    table[std::to_integer<std::uint8_t>(b)] = {.type = Prefixtype::GROUP2, .is_prefix = true};
  }

  table[std::to_integer<std::uint8_t>(LegacyGroup3Prefix)] = {.type = Prefixtype::GROUP3, .is_prefix = true};
  table[std::to_integer<std::uint8_t>(LegacyGroup4Prefix)] = {.type = Prefixtype::GROUP4, .is_prefix = true};

  for(auto b : RexPrefix)
  {
    table[std::to_integer<std::uint8_t>(b)] = {.type = Prefixtype::REX, .is_prefix = true};
  }

  return table;
}

static constexpr std::array<PrefixMetadata, 256> PREFIX_TABLE = build_prefix_table();

// test data
// 48 8B 44 8C 10 => MOV RAX, [RSP + RCX * 4 + 0x10]
constexpr std::array<std::byte, 4> TestModRMOpcode{std::byte{0x8B}, std::byte{0x44}, std::byte{0x8C}, std::byte{0x10}};

inline auto TestBytesSpan = std::span{TestModRMOpcode};