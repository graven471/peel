#pragma once

#include <array>
#include <cstddef>

/*
0F        -> escape from the primary opcode map
0F XX     -> 0F opcode map

0F 38 XX  -> 0F 38 opcode map
0F 3A XX  -> 0F 3A opcode map
*/

// signals this isn't a 1-byte opcode
constexpr std::byte OPCODE_ESCAPE = std::byte{0x0F};
// 0F 38 XX opcode map
constexpr std::byte OPCODE_MAP2_SELECT = std::byte{0x38};
// OF 3A XX opcode map
constexpr std::byte OPCODE_MAP3_SELECT = std::byte{0x3A};

constexpr std::array<std::byte, 2> BuildTwoBytesOpcode(
    const std::byte Opcode) noexcept {
  return std::array{OPCODE_ESCAPE, Opcode};
}

constexpr std::array<std::byte, 3> BuildOpcodeMap2(
    const std::byte Opcode) noexcept {
  return std::array{OPCODE_ESCAPE, OPCODE_MAP2_SELECT, Opcode};
}

constexpr std::array<std::byte, 3> BuildOpcodeMap3(
    const std::byte Opcode) noexcept {
  return std::array{OPCODE_ESCAPE, OPCODE_MAP3_SELECT, Opcode};
}
