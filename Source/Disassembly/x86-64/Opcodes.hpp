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

enum class Mnemonic : uint16_t
{
  INVALID,

  MOV,
  MOVS,
  MOVSXD,
  MOVBE,

  RET,
  RETF,

  ADD,
  SUB,
  IMUL,

  ADC,

  OR,
  AND,
  XOR,

  INC,
  DEC,

  CMP,
  PUSH,
  PUSHAD,

  POP,
  POPAD,

  TEST,

  INT,
  INT1,
  INT3,
  INTO,
  IRET,

  IN,
  OUT,

  JO,
  JNO,
  JB,
  JNB,
  JZ,
  JNZ,
  JBE,
  JA,
  JS,
  JNS,
  JP,
  JNP,
  JL,
  JGE,
  JLE,
  JG,

  JMP,
  JRCXZ,
  CALL,

  WAIT,

  BOUND,

  XCHG,
  XLAT,

  NOP,
  HLT,
  CMC,
  CLC,
  STC,
  CLI,
  STI,
  CLD,
  STD,

  CMPS,
  LODS,
  STOS,
  SCAS,

  LOOP,
  LOOPE,
  LOOPNE,

  ENTER,
  LEAVE,

  ROL,
  ROR,
  RCL,
  RCR,
  SHL,
  SHR,
  SAR,
  NEG,
  NOT,
  MUL,
  DIV,
  IDIV,

  SEG_ES,
  SEG_CS,
  SEG_SS,
  SEG_DS,
  SEG_FS,
  SEG_GS,

  OPERAND_SIZE_PREFIX,
  ADDRESS_SIZE_PREFIX,
  LOCK,
  REP,
  REPNE,

  VEX1,
  VEX2,
  X87,

  SBB,

  DAA,
  AAA,

  GROUP1,
  GROUP2,
  GROUP3,
  GROUP4,
  GROUP5,
  GROUP11,
};

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

  SS,

  // jump target dst in Jmp Mnemonic
  Jb,

  Ob,
  Ov,
  Yb,
  Xb,
  Yv,
  Xv,

  AL,
  CL,
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
    Mnemonic::ROL, Mnemonic::ROR, Mnemonic::RCL, Mnemonic::RCR,
    Mnemonic::SHL, Mnemonic::SHL, Mnemonic::SHR, Mnemonic::SAR,
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

// Mnemonic dst, src
struct InstructionDesc
{
  Mnemonic    mnemonic;
  OperandCode Destination;
  OperandCode Source;
};

static constexpr InstructionDesc OPCODE_TABLE[256] = {
    // 0x00
    {.mnemonic = Mnemonic::ADD, .Destination = OperandCode::Eb, .Source = OperandCode::Gb},
    {.mnemonic = Mnemonic::ADD, .Destination = OperandCode::Ev, .Source = OperandCode::Gv},
    {.mnemonic = Mnemonic::ADD, .Destination = OperandCode::Gb, .Source = OperandCode::Eb},
    {.mnemonic = Mnemonic::ADD, .Destination = OperandCode::Gv, .Source = OperandCode::Ev},
    {.mnemonic = Mnemonic::ADD, .Destination = OperandCode::AL, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::ADD, .Destination = OperandCode::rAX, .Source = OperandCode::Iz},
    {.mnemonic = Mnemonic::PUSH, .Destination = OperandCode::Es, .Source = OperandCode::None},
    // 0x7
    {.mnemonic = Mnemonic::POP, .Destination = OperandCode::Es, .Source = OperandCode::None},

    // 0x10
    {.mnemonic = Mnemonic::ADC, .Destination = OperandCode::Eb, .Source = OperandCode::Gb},
    {.mnemonic = Mnemonic::ADC, .Destination = OperandCode::Ev, .Source = OperandCode::Gv},
    {.mnemonic = Mnemonic::ADC, .Destination = OperandCode::Gb, .Source = OperandCode::Eb},
    {.mnemonic = Mnemonic::ADC, .Destination = OperandCode::Gv, .Source = OperandCode::Ev},
    {.mnemonic = Mnemonic::ADC, .Destination = OperandCode::AL, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::ADC, .Destination = OperandCode::rAX, .Source = OperandCode::Iz},
    {.mnemonic = Mnemonic::PUSH, .Destination = OperandCode::SS, .Source = OperandCode::None},
    // 0x17
    {.mnemonic = Mnemonic::POP, .Destination = OperandCode::SS, .Source = OperandCode::None},

    {.mnemonic = Mnemonic::AND, .Destination = OperandCode::Eb, .Source = OperandCode::Gb},
    {.mnemonic = Mnemonic::AND, .Destination = OperandCode::Ev, .Source = OperandCode::Gv},
    {.mnemonic = Mnemonic::AND, .Destination = OperandCode::Gb, .Source = OperandCode::Eb},
    {.mnemonic = Mnemonic::AND, .Destination = OperandCode::Gv, .Source = OperandCode::Ev},
    {.mnemonic = Mnemonic::AND, .Destination = OperandCode::AL, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::AND, .Destination = OperandCode::rAX, .Source = OperandCode::Iz},
    {.mnemonic = Mnemonic::SEG_ES, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0x27
    {.mnemonic = Mnemonic::DAA, .Destination = OperandCode::None, .Source = OperandCode::None},

    {.mnemonic = Mnemonic::XOR, .Destination = OperandCode::Eb, .Source = OperandCode::Gb},
    {.mnemonic = Mnemonic::XOR, .Destination = OperandCode::Ev, .Source = OperandCode::Gv},
    {.mnemonic = Mnemonic::XOR, .Destination = OperandCode::Gb, .Source = OperandCode::Eb},
    {.mnemonic = Mnemonic::XOR, .Destination = OperandCode::Gv, .Source = OperandCode::Ev},
    {.mnemonic = Mnemonic::XOR, .Destination = OperandCode::AL, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::XOR, .Destination = OperandCode::rAX, .Source = OperandCode::Iz},
    {.mnemonic = Mnemonic::SEG_SS, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0x37
    {.mnemonic = Mnemonic::DAA, .Destination = OperandCode::None, .Source = OperandCode::None},

    // INC^i64 general register / REX^o64 prefixes only valid in amd64 mode
    {.mnemonic = Mnemonic::INC, .Destination = OperandCode::eAX, .Source = OperandCode::REX},
    // REX.B
    {.mnemonic = Mnemonic::INC, .Destination = OperandCode::eCX, .Source = OperandCode::REX},
    // REX.X
    {.mnemonic = Mnemonic::INC, .Destination = OperandCode::eDX, .Source = OperandCode::REX},
    // REX.XB
    {.mnemonic = Mnemonic::INC, .Destination = OperandCode::eBX, .Source = OperandCode::REX},
    // REX.R
    {.mnemonic = Mnemonic::INC, .Destination = OperandCode::eSP, .Source = OperandCode::REX},
    // REX.RB
    {.mnemonic = Mnemonic::INC, .Destination = OperandCode::eBP, .Source = OperandCode::REX},
    // REX.RX
    {.mnemonic = Mnemonic::INC, .Destination = OperandCode::eSI, .Source = OperandCode::REX},
    // REX.RXB
    // 0x47
    {.mnemonic = Mnemonic::INC, .Destination = OperandCode::eDI, .Source = OperandCode::REX},

    // PUSH^d64 general register
    {.mnemonic = Mnemonic::PUSH, .Destination = OperandCode::rCX, .Source = OperandCode::R9},
    {.mnemonic = Mnemonic::PUSH, .Destination = OperandCode::rDX, .Source = OperandCode::R10},
    {.mnemonic = Mnemonic::PUSH, .Destination = OperandCode::rBX, .Source = OperandCode::R11},
    {.mnemonic = Mnemonic::PUSH, .Destination = OperandCode::rSP, .Source = OperandCode::R12},
    {.mnemonic = Mnemonic::PUSH, .Destination = OperandCode::rBP, .Source = OperandCode::R13},
    {.mnemonic = Mnemonic::PUSH, .Destination = OperandCode::rSI, .Source = OperandCode::R14},
    // 0x57
    {.mnemonic = Mnemonic::PUSH, .Destination = OperandCode::rDI, .Source = OperandCode::R15},

    {.mnemonic = Mnemonic::PUSHAD, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::POPAD, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::BOUND, .Destination = OperandCode::Gv, .Source = OperandCode::Ma},
    // note only valid in amd64 in intel32 mode its ARPL i am focusing mainly on AMD64
    {.mnemonic = Mnemonic::MOVSXD, .Destination = OperandCode::Gv, .Source = OperandCode::Ev},
    // SEG = FS (Prefix)
    {.mnemonic = Mnemonic::SEG_FS, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::SEG_GS, .Destination = OperandCode::None, .Source = OperandCode::None},
    // Operand/Address size (prefix)
    {.mnemonic = Mnemonic::OPERAND_SIZE_PREFIX, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0x67
    {.mnemonic = Mnemonic::ADDRESS_SIZE_PREFIX, .Destination = OperandCode::None, .Source = OperandCode::None},

    // Jcc^f64, jb - short-displacement jump on condition
    {.mnemonic = Mnemonic::JO, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::JNO, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::JB, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::JNB, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::JZ, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::JNZ, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::JBE, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    // 0x77
    {.mnemonic = Mnemonic::JA, .Destination = OperandCode::Jb, .Source = OperandCode::None},

    // for GROUP1 Bits 5, 4, and 3 of ModR/M byte used as an opcode extension
    {.mnemonic = Mnemonic::GROUP1, .Destination = OperandCode::Eb, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::GROUP1, .Destination = OperandCode::Ev, .Source = OperandCode::Iz},
    // invalid in amd64
    {.mnemonic = Mnemonic::GROUP1, .Destination = OperandCode::Eb, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::GROUP1, .Destination = OperandCode::Ev, .Source = OperandCode::Ib},

    {.mnemonic = Mnemonic::TEST, .Destination = OperandCode::Eb, .Source = OperandCode::Gb},
    {.mnemonic = Mnemonic::TEST, .Destination = OperandCode::Ev, .Source = OperandCode::Gv},
    {.mnemonic = Mnemonic::XCHG, .Destination = OperandCode::Eb, .Source = OperandCode::Gb},
    // 0x87
    {.mnemonic = Mnemonic::XCHG, .Destination = OperandCode::Ev, .Source = OperandCode::Gv},

    // XCHG word, double-word or quad-word register with rAX

    // this can be either NOP or PAUSE(F3) or XCHG let prefix scanner resolve it
    {.mnemonic = Mnemonic::NOP, .Destination = OperandCode::R8, .Source = OperandCode::rAX},
    {.mnemonic = Mnemonic::XCHG, .Destination = OperandCode::rCX, .Source = OperandCode::R9},
    {.mnemonic = Mnemonic::XCHG, .Destination = OperandCode::rDX, .Source = OperandCode::R10},
    {.mnemonic = Mnemonic::XCHG, .Destination = OperandCode::rBX, .Source = OperandCode::R11},
    {.mnemonic = Mnemonic::XCHG, .Destination = OperandCode::rSP, .Source = OperandCode::R12},
    {.mnemonic = Mnemonic::XCHG, .Destination = OperandCode::rBP, .Source = OperandCode::R13},
    {.mnemonic = Mnemonic::XCHG, .Destination = OperandCode::rSI, .Source = OperandCode::R14},
    // 0x97
    {.mnemonic = Mnemonic::XCHG, .Destination = OperandCode::rDI, .Source = OperandCode::R15},

    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::AL, .Source = OperandCode::Ob},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::rAX, .Source = OperandCode::Ov},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::Ob, .Source = OperandCode::AL},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::Ov, .Source = OperandCode::rAX},
    // MOVS/B
    {.mnemonic = Mnemonic::MOVS, .Destination = OperandCode::Yb, .Source = OperandCode::Xb},
    // MOVS/W/D/Q
    {.mnemonic = Mnemonic::MOVS, .Destination = OperandCode::Yv, .Source = OperandCode::Xv},
    // CMPS/B
    {.mnemonic = Mnemonic::CMPS, .Destination = OperandCode::Yb, .Source = OperandCode::Xb},
    // CMPS/W/D/Q
    // 0xA7
    {.mnemonic = Mnemonic::CMPS, .Destination = OperandCode::Yv, .Source = OperandCode::Xv},

    // MOV immediate byte into byte register
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::AL, .Source = OperandCode::R8B},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::CL, .Source = OperandCode::R9B},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::DL, .Source = OperandCode::R10B},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::BL, .Source = OperandCode::R11B},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::AH, .Source = OperandCode::R12B},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::CH, .Source = OperandCode::R13B},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::DH, .Source = OperandCode::R14B},
    // 0xB7
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::BH, .Source = OperandCode::R15B},

    // Shift Group2^1A, 1A = Bits 5, 4, and 3 of MODR/M byte used as an opcode extension
    {.mnemonic = Mnemonic::GROUP2, .Destination = OperandCode::Eb, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::GROUP2, .Destination = OperandCode::Ev, .Source = OperandCode::Ib},

    // near RET^f64, f64 = operand size is forced to 64-bit bit operand size when in AMD64 mode
    {.mnemonic = Mnemonic::RET, .Destination = OperandCode::Iw, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::RET, .Destination = OperandCode::None, .Source = OperandCode::None},

    // 0xC4
    {.mnemonic = Mnemonic::VEX2, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xC5
    {.mnemonic = Mnemonic::VEX1, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xC6
    {.mnemonic = Mnemonic::GROUP11, .Destination = OperandCode::Eb, .Source = OperandCode::Ib},
    // 0xC7
    {.mnemonic = Mnemonic::GROUP11, .Destination = OperandCode::Ev, .Source = OperandCode::Iz},
    // 0xC8
    {.mnemonic = Mnemonic::ENTER, .Destination = OperandCode::Iw, .Source = OperandCode::Ib},
    // 0xC9
    {.mnemonic = Mnemonic::LEAVE, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xCA
    {.mnemonic = Mnemonic::RETF, .Destination = OperandCode::Iw, .Source = OperandCode::None},
    // 0xCB
    {.mnemonic = Mnemonic::RETF, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xCC
    {.mnemonic = Mnemonic::INT3, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xCD
    {.mnemonic = Mnemonic::INT, .Destination = OperandCode::Ib, .Source = OperandCode::None},
    // 0xCE
    {.mnemonic = Mnemonic::INVALID, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xCF
    {.mnemonic = Mnemonic::IRET, .Destination = OperandCode::None, .Source = OperandCode::None},

    // 0xD0
    {.mnemonic = Mnemonic::GROUP2, .Destination = OperandCode::Eb, .Source = OperandCode::One},
    // 0xD1
    {.mnemonic = Mnemonic::GROUP2, .Destination = OperandCode::Ev, .Source = OperandCode::One},
    // 0xD2
    {.mnemonic = Mnemonic::GROUP2, .Destination = OperandCode::Eb, .Source = OperandCode::CL},
    // 0xD3
    {.mnemonic = Mnemonic::GROUP2, .Destination = OperandCode::Ev, .Source = OperandCode::CL},
    // 0xD4
    {.mnemonic = Mnemonic::INVALID, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xD5
    {.mnemonic = Mnemonic::INVALID, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xD6
    {.mnemonic = Mnemonic::INVALID, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xD7
    {.mnemonic = Mnemonic::XLAT, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xD8
    {.mnemonic = Mnemonic::X87, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xD9
    {.mnemonic = Mnemonic::X87, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xDA
    {.mnemonic = Mnemonic::X87, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xDB
    {.mnemonic = Mnemonic::X87, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xDC
    {.mnemonic = Mnemonic::X87, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xDD
    {.mnemonic = Mnemonic::X87, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xDE
    {.mnemonic = Mnemonic::X87, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xDF
    {.mnemonic = Mnemonic::X87, .Destination = OperandCode::None, .Source = OperandCode::None},

    // 0xE0
    {.mnemonic = Mnemonic::LOOPNE, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    // 0xE1
    {.mnemonic = Mnemonic::LOOPE, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    // 0xE2
    {.mnemonic = Mnemonic::LOOP, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    // 0xE3
    {.mnemonic = Mnemonic::JRCXZ, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    // 0xE4
    {.mnemonic = Mnemonic::IN, .Destination = OperandCode::AL, .Source = OperandCode::Ib},
    // 0xE5
    {.mnemonic = Mnemonic::IN, .Destination = OperandCode::eAX, .Source = OperandCode::Ib},
    // 0xE6
    {.mnemonic = Mnemonic::OUT, .Destination = OperandCode::Ib, .Source = OperandCode::AL},
    // 0xE7
    {.mnemonic = Mnemonic::OUT, .Destination = OperandCode::Ib, .Source = OperandCode::eAX},
    // 0xE8
    {.mnemonic = Mnemonic::CALL, .Destination = OperandCode::Jz, .Source = OperandCode::None},
    // 0xE9
    {.mnemonic = Mnemonic::JMP, .Destination = OperandCode::Jz, .Source = OperandCode::None},
    // 0xEA
    {.mnemonic = Mnemonic::INVALID, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xEB
    {.mnemonic = Mnemonic::JMP, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    // 0xEC
    {.mnemonic = Mnemonic::IN, .Destination = OperandCode::AL, .Source = OperandCode::DX},
    // 0xED
    {.mnemonic = Mnemonic::IN, .Destination = OperandCode::eAX, .Source = OperandCode::DX},
    // 0xEE
    {.mnemonic = Mnemonic::OUT, .Destination = OperandCode::DX, .Source = OperandCode::AL},
    // 0xEF
    {.mnemonic = Mnemonic::OUT, .Destination = OperandCode::DX, .Source = OperandCode::eAX},

    // 0xF0
    {.mnemonic = Mnemonic::LOCK, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xF1
    {.mnemonic = Mnemonic::INT1, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xF2
    {.mnemonic = Mnemonic::REPNE, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xF3
    {.mnemonic = Mnemonic::REP, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xF4
    {.mnemonic = Mnemonic::HLT, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xF5
    {.mnemonic = Mnemonic::CMC, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xF6
    {.mnemonic = Mnemonic::GROUP3, .Destination = OperandCode::Eb, .Source = OperandCode::None},
    // 0xF7
    {.mnemonic = Mnemonic::GROUP3, .Destination = OperandCode::Ev, .Source = OperandCode::None},
    // 0xF8
    {.mnemonic = Mnemonic::CLC, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xF9
    {.mnemonic = Mnemonic::STC, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xFA
    {.mnemonic = Mnemonic::CLI, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xFB
    {.mnemonic = Mnemonic::STI, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xFC
    {.mnemonic = Mnemonic::CLD, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xFD
    {.mnemonic = Mnemonic::STD, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xFE
    {.mnemonic = Mnemonic::GROUP4, .Destination = OperandCode::Eb, .Source = OperandCode::None},
    // 0xFF
    {.mnemonic = Mnemonic::GROUP5, .Destination = OperandCode::Ev, .Source = OperandCode::None},

    // remaining 126 entries

};