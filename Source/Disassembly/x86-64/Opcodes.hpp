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

enum class OperandSize : std::uint8_t
{
  // real mode
  Bits8,
  Bits16,
  // protected mode
  Bits32,
  // amd64 mode
  Bits64
};

/*
THANKS A LOT Intel For making all of these info available freely ^^
Intel SDM Volume 2 Appendix A and B
*/

#define OPERAND_CODE_LIST(X)                                                                                           \
  X(None)                                                                                                              \
  X(Eb)                                                                                                                \
  X(Es)                                                                                                                \
  X(Gb)                                                                                                                \
  X(Ev)                                                                                                                \
  X(Gv)                                                                                                                \
  X(Ma)                                                                                                                \
  X(AL)                                                                                                                \
  X(Q)                                                                                                                 \
  X(W)                                                                                                                 \
  X(N)                                                                                                                 \
  X(CL)                                                                                                                \
  X(Fv)                                                                                                                \
  X(Ew)                                                                                                                \
  X(Ib)                                                                                                                \
  X(Iw)                                                                                                                \
  X(Iv)                                                                                                                \
  X(Iz)                                                                                                                \
  X(eAX)                                                                                                               \
  X(eBX)                                                                                                               \
  X(eBP)                                                                                                               \
  X(eCX)                                                                                                               \
  X(eDX)                                                                                                               \
  X(eDI)                                                                                                               \
  X(eSP)                                                                                                               \
  X(eSI)                                                                                                               \
  X(R8)                                                                                                                \
  X(R8B)                                                                                                               \
  X(R9)                                                                                                                \
  X(R9B)                                                                                                               \
  X(R10)                                                                                                               \
  X(R10B)                                                                                                              \
  X(R11)                                                                                                               \
  X(R11B)                                                                                                              \
  X(R12)                                                                                                               \
  X(R12B)                                                                                                              \
  X(R13)                                                                                                               \
  X(R13B)                                                                                                              \
  X(R14)                                                                                                               \
  X(R14B)                                                                                                              \
  X(R15)                                                                                                               \
  X(R15B)                                                                                                              \
  X(REX)                                                                                                               \
  X(rAX)                                                                                                               \
  X(rBX)                                                                                                               \
  X(rCX)                                                                                                               \
  X(rDX)                                                                                                               \
  X(rSP)                                                                                                               \
  X(rBP)                                                                                                               \
  X(rSI)                                                                                                               \
  X(rDI)                                                                                                               \
  X(Sw)                                                                                                                \
  X(M)                                                                                                                 \
  X(Ap)                                                                                                                \
  X(SS)                                                                                                                \
  X(DS)                                                                                                                \
  X(CS)                                                                                                                \
  X(FS)                                                                                                                \
  X(GS)                                                                                                                \
  X(Jb)                                                                                                                \
  X(Ob)                                                                                                                \
  X(Ov)                                                                                                                \
  X(Oz)                                                                                                                \
  X(Yb)                                                                                                                \
  X(Yz)                                                                                                                \
  X(Xb)                                                                                                                \
  X(Yv)                                                                                                                \
  X(Xv)                                                                                                                \
  X(Xz)                                                                                                                \
  X(DL)                                                                                                                \
  X(BL)                                                                                                                \
  X(HL)                                                                                                                \
  X(AH)                                                                                                                \
  X(CH)                                                                                                                \
  X(DH)                                                                                                                \
  X(BH)                                                                                                                \
  X(DX)                                                                                                                \
  X(Jz)                                                                                                                \
  X(One)

enum class OperandCode : std::uint16_t
{
#define X(name) name,
  OPERAND_CODE_LIST(X)
#undef X
};

constexpr std::string_view ToString(OperandCode Value) noexcept
{
  switch(Value)
  {
#define X(name)                                                                                                        \
  case OperandCode::name:                                                                                              \
    return #name;

    OPERAND_CODE_LIST(X)

#undef X
  }

  return "Unknown";
}

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
