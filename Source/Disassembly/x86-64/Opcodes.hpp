#pragma once

#include <array>
#include <cstddef>
#include "Mnemonic.hpp"

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

constexpr std::array<std::byte, 2> BuildTwoBytesOpcode(const std::byte Opcode) noexcept
{
  return std::array{OPCODE_ESCAPE, Opcode};
}

constexpr std::array<std::byte, 3> BuildOpcodeMap2(const std::byte Opcode) noexcept
{
  return std::array{OPCODE_ESCAPE, OPCODE_MAP2_SELECT, Opcode};
}

constexpr std::array<std::byte, 3> BuildOpcodeMap3(const std::byte Opcode) noexcept
{
  return std::array{OPCODE_ESCAPE, OPCODE_MAP3_SELECT, Opcode};
}

/*
THANKS A LOT Intel For making all of these info available freely ^^
Intel SDM Volume 2 Appendix A and B
*/

enum class OperandCode : uint16_t
{
  None,
  Eb,
  Es,
  Gb,
  Ev,
  Gv,
  Ma,
  AL,

  // MMX register
  Q,

  // XMM register
  W,

  // MMX register
  N,

  CL,

  Fv,

  Ew,

  Ib,
  Iw,
  Iv,
  Iz,

  eAX,
  eBX,
  eBP,
  eCX,
  eDX,
  eDI,
  eSP,
  eSI,

  R8,
  R8B,

  R9,
  R9B,

  R10,
  R10B,

  R11,
  R11B,

  R12,
  R12B,

  R13,
  R13B,

  R14,
  R14B,

  R15,
  R15B,

  REX,
  rAX,
  rBX,
  rCX,
  rDX,
  rSP,
  rBP,
  rSI,
  rDI,

  Sw,
  M,
  Ap,


  // segment registers
  SS,
  DS,
  CS,
  FS,
  GS,

  // jump target dst in Jmp Mnemonic
  Jb,

  Ob,
  Ov,
  Oz,
  Yb,
  Yz,
  Xb,
  Yv,
  Xv,
  Xz,

  DL,
  BL,
  HL,
  AH,
  CH,
  DH,
  BH,

  DX,
  Jz,
  One,
};

// opcode 0x80-0x83
static constexpr Mnemonic GROUP1[8] = {
    Mnemonic::ADD, Mnemonic::OR,  Mnemonic::ADC, Mnemonic::SBB,
    Mnemonic::AND, Mnemonic::SUB, Mnemonic::XOR, Mnemonic::CMP,
};

// opcode 0x8F
static constexpr Mnemonic GROUP1A[8] = {
    Mnemonic::POP,      // 000
    Mnemonic::INVALID,  // 001
    Mnemonic::INVALID,  // 010
    Mnemonic::INVALID,  // 011
    Mnemonic::INVALID,  // 100
    Mnemonic::INVALID,  // 101
    Mnemonic::INVALID,  // 110
    Mnemonic::INVALID,  // 111
};

static constexpr Mnemonic GROUP2[8] = {
    Mnemonic::ROL,  // 000
    Mnemonic::ROR,  // 001
    Mnemonic::RCL,  // 010
    Mnemonic::RCR,  // 011
    Mnemonic::SHL,  // 100
    Mnemonic::SHL,  // 101
    Mnemonic::SHR,  // 110
    Mnemonic::SAR,  // 111
};

static constexpr Mnemonic GROUP3[8] = {
    Mnemonic::TEST,     // 000
    Mnemonic::INVALID,  // 001
    Mnemonic::NOT,      // 010
    Mnemonic::NEG,      // 011
    Mnemonic::MUL,      // 100
    Mnemonic::IMUL,     // 101
    Mnemonic::DIV,      // 110
    Mnemonic::IDIV,     // 111
};

static constexpr Mnemonic GROUP4[8] = {
    Mnemonic::INC,      // 000
    Mnemonic::DEC,      // 001
    Mnemonic::INVALID,  // 010
    Mnemonic::INVALID,  // 011
    Mnemonic::INVALID,  // 100
    Mnemonic::INVALID,  // 101
    Mnemonic::INVALID,  // 110
    Mnemonic::INVALID,  // 111
};

static constexpr Mnemonic GROUP5[8] = {
    Mnemonic::INC,      // 000
    Mnemonic::DEC,      // 001
    Mnemonic::CALL,     // 010
    Mnemonic::CALL,     // 011 far call
    Mnemonic::JMP,      // 100
    Mnemonic::JMP,      // 101 far jmp
    Mnemonic::PUSH,     // 110
    Mnemonic::INVALID,  // 111
};

static constexpr Mnemonic GROUP11[8] = {
    Mnemonic::MOV,      // 000
    Mnemonic::INVALID,  // 001
    Mnemonic::INVALID,  // 010
    Mnemonic::INVALID,  // 011
    Mnemonic::INVALID,  // 100
    Mnemonic::INVALID,  // 101
    Mnemonic::INVALID,  // 110
    Mnemonic::INVALID,  // 111
};
