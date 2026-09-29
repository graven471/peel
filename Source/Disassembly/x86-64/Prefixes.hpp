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

constexpr std::array rex_prefix = {std::byte{0x40}, std::byte{0x41}, std::byte{0x42}, std::byte{0x43},
                                   std::byte{0x44}, std::byte{0x45}, std::byte{0x46}, std::byte{0x47},
                                   std::byte{0x48}, std::byte{0x49}, std::byte{0x4A}, std::byte{0x4B},
                                   std::byte{0x4C}, std::byte{0x4D}, std::byte{0x4E}, std::byte{0x4F}};

constexpr std::array legacy_group1_prefixes = {std::byte{0xF0}, std::byte{0xF2}, std::byte{0xF3}};

constexpr std::array legacy_group2_prefixes = {std::byte{0x2E}, std::byte{0x36}, std::byte{0x3E},
                                               std::byte{0x26}, std::byte{0x64}, std::byte{0x65}};

constexpr std::byte legacy_group3_prefix = std::byte{0x66};
constexpr std::byte legacy_group4_prefix = std::byte{0x67};

enum class PrefixType : std::uint8_t
{
  Group1,
  Group2,
  Group3,
  Group4,
  Rex,
  None
};

struct PrefixMetadata
{
  PrefixType type{PrefixType::None};
};

static consteval std::array<PrefixMetadata, 256> build_prefix_table() noexcept
{
  std::array<PrefixMetadata, 256> table{};

  for(auto byte : legacy_group1_prefixes)
  {
    table[std::to_integer<std::uint8_t>(byte)] = {.type = PrefixType::Group1};
  }

  for(auto byte : legacy_group2_prefixes)
  {
    table[std::to_integer<std::uint8_t>(byte)] = {.type = PrefixType::Group2};
  }

  table[std::to_integer<std::uint8_t>(legacy_group3_prefix)] = {.type = PrefixType::Group3};
  table[std::to_integer<std::uint8_t>(legacy_group4_prefix)] = {.type = PrefixType::Group4};

  for(auto byte : rex_prefix)
  {
    table[std::to_integer<std::uint8_t>(byte)] = {.type = PrefixType::Rex};
  }

  return table;
}

static constexpr std::array<PrefixMetadata, 256> prefix_table = build_prefix_table();

// test data
// 48 8B 44 8C 10 => MOV RAX, [RSP + RCX * 4 + 0x10]
constexpr std::array<std::byte, 4> test_mod_rm_opcode = {std::byte{0x8B}, std::byte{0x44}, std::byte{0x8C}, std::byte{0x10}};

inline auto test_bytes_span = std::span{test_mod_rm_opcode};