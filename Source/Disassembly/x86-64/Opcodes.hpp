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

/*
THANKS A LOT Intel For making all of these info available freely ^^
Intel SDM Volume 2 Appendix A and B
*/

enum class Mnemonic : uint16_t
{
  INVALID,

  MOV,
  MOVS,
  MOVSXD,
  MOVBE,

  ESCAPE_2BYTE,

  // load-effective-address
  LEA,

  RET,
  RETF,

  ADD,
  SUB,
  IMUL,

  CBW,
  CWD,


  INS,

  ADC,

  OR,
  AND,
  XOR,

  INC,
  DEC,

  CMP,
  PUSH,
  PUSHF,
  PUSHAD,

  POP,
  POPF,
  POPAD,

  TEST,

  INT,
  INT1,
  INT3,
  INTO,
  IRET,

  IN,
  OUT,
  OUTS,

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

  SAHF,
  LAHF,

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
  DAS,
  AAA,
  AAS,

  GROUP1,
  GROUP1A,
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

  // MMX register
  Q,

  // XMM register
  W,

  // MMX register
  N,

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
  Yz,
  Xv,
  Xz,

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

enum class ModRMRegMode : uint8_t
{
  None,
  Register,        // /r
  OpcodeExtension  // /0 through /7
};

// Mnemonic dst, src
struct InstructionDesc
{
  Mnemonic     mnemonic;
  OperandCode  Destination;
  OperandCode  Source;
  OperandCode  Extra    = OperandCode::None;
  bool         ModRM    = false;
  ModRMRegMode RegField = ModRMRegMode::None;
  std::uint8_t RegExtension;
};

// E, G, C, M, D, M, Q, R, S, U, V, W needs ModR/M
// E  -> ModR/M.r/m
// G  -> ModR/M.reg
// C  -> ModR/M.reg (control register)
// D  -> ModR/M.reg (debug register)
// b/v/w/d/q/etc. -> describe the operand's size/type
static constexpr InstructionDesc OPCODE_TABLE[256] = {
    // 0x00
    {.mnemonic = Mnemonic::ADD, .Destination = OperandCode::Eb, .Source = OperandCode::Gb, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::ADD, .Destination = OperandCode::Ev, .Source = OperandCode::Gv, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::ADD, .Destination = OperandCode::Gb, .Source = OperandCode::Eb, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::ADD, .Destination = OperandCode::Gv, .Source = OperandCode::Ev, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::ADD, .Destination = OperandCode::AL, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::ADD, .Destination = OperandCode::rAX, .Source = OperandCode::Iz},
    {.mnemonic = Mnemonic::PUSH, .Destination = OperandCode::Es, .Source = OperandCode::None},
    // 0x7
    {.mnemonic = Mnemonic::POP, .Destination = OperandCode::Es, .Source = OperandCode::None},

    // 0x08
    {.mnemonic = Mnemonic::OR, .Destination = OperandCode::Eb, .Source = OperandCode::Gb, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::OR, .Destination = OperandCode::Ev, .Source = OperandCode::Gv, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::OR, .Destination = OperandCode::Gb, .Source = OperandCode::Eb, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::OR, .Destination = OperandCode::Gv, .Source = OperandCode::Ev, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::OR, .Destination = OperandCode::AL, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::OR, .Destination = OperandCode::rAX, .Source = OperandCode::Iz},
    {.mnemonic = Mnemonic::PUSH, .Destination = OperandCode::CS, .Source = OperandCode::None},
    // 0x0F
    {.mnemonic = Mnemonic::ESCAPE_2BYTE, .Destination = OperandCode::None, .Source = OperandCode::None},

    // 0x10
    {.mnemonic = Mnemonic::ADC, .Destination = OperandCode::Eb, .Source = OperandCode::Gb, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::ADC, .Destination = OperandCode::Ev, .Source = OperandCode::Gv, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::ADC, .Destination = OperandCode::Gb, .Source = OperandCode::Eb, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::ADC, .Destination = OperandCode::Gv, .Source = OperandCode::Ev, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::ADC, .Destination = OperandCode::AL, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::ADC, .Destination = OperandCode::rAX, .Source = OperandCode::Iz},
    {.mnemonic = Mnemonic::PUSH, .Destination = OperandCode::SS, .Source = OperandCode::None},
    // 0x17
    {.mnemonic = Mnemonic::POP, .Destination = OperandCode::SS, .Source = OperandCode::None},

    // 0x18
    {.mnemonic = Mnemonic::SBB, .Destination = OperandCode::Eb, .Source = OperandCode::Gb, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::SBB, .Destination = OperandCode::Ev, .Source = OperandCode::Gv, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::SBB, .Destination = OperandCode::Gb, .Source = OperandCode::Eb, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::SBB, .Destination = OperandCode::Gv, .Source = OperandCode::Ev, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::SBB, .Destination = OperandCode::AL, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::SBB, .Destination = OperandCode::rAX, .Source = OperandCode::Iz},
    // only valid in amd64 mode
    {.mnemonic = Mnemonic::PUSH, .Destination = OperandCode::DS, .Source = OperandCode::None},
    // 0xF
    {.mnemonic = Mnemonic::POP, .Destination = OperandCode::DS, .Source = OperandCode::None},

    // 0x20
    {.mnemonic = Mnemonic::AND, .Destination = OperandCode::Eb, .Source = OperandCode::Gb, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::AND, .Destination = OperandCode::Ev, .Source = OperandCode::Gv, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::AND, .Destination = OperandCode::Gb, .Source = OperandCode::Eb, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::AND, .Destination = OperandCode::Gv, .Source = OperandCode::Ev, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::AND, .Destination = OperandCode::AL, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::AND, .Destination = OperandCode::rAX, .Source = OperandCode::Iz},
    {.mnemonic = Mnemonic::SEG_ES, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0x27
    {.mnemonic = Mnemonic::DAA, .Destination = OperandCode::None, .Source = OperandCode::None},

    // 0x28
    {.mnemonic = Mnemonic::SUB, .Destination = OperandCode::Eb, .Source = OperandCode::Gb, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::SUB, .Destination = OperandCode::Ev, .Source = OperandCode::Gv, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::SUB, .Destination = OperandCode::Gb, .Source = OperandCode::Eb, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::SUB, .Destination = OperandCode::Gv, .Source = OperandCode::Ev, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::SUB, .Destination = OperandCode::AL, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::SUB, .Destination = OperandCode::rAX, .Source = OperandCode::Iz},
    {.mnemonic = Mnemonic::SEG_CS, .Destination = OperandCode::None, .Source = OperandCode::None},
    // only valid in amd64
    // 0x2F
    {.mnemonic = Mnemonic::DAS, .Destination = OperandCode::None, .Source = OperandCode::None},

    // 0x30
    {.mnemonic = Mnemonic::XOR, .Destination = OperandCode::Eb, .Source = OperandCode::Gb, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::XOR, .Destination = OperandCode::Ev, .Source = OperandCode::Gv, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::XOR, .Destination = OperandCode::Gb, .Source = OperandCode::Eb, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::XOR, .Destination = OperandCode::Gv, .Source = OperandCode::Ev, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::XOR, .Destination = OperandCode::AL, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::XOR, .Destination = OperandCode::rAX, .Source = OperandCode::Iz},
    {.mnemonic = Mnemonic::SEG_SS, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0x37
    {.mnemonic = Mnemonic::AAA, .Destination = OperandCode::None, .Source = OperandCode::None},

    // 0x38
    {.mnemonic = Mnemonic::CMP, .Destination = OperandCode::Eb, .Source = OperandCode::Gb, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::CMP, .Destination = OperandCode::Ev, .Source = OperandCode::Gv, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::CMP, .Destination = OperandCode::Gb, .Source = OperandCode::Eb, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::CMP, .Destination = OperandCode::Gv, .Source = OperandCode::Ev, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::CMP, .Destination = OperandCode::AL, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::CMP, .Destination = OperandCode::rAX, .Source = OperandCode::Iz},
    {.mnemonic = Mnemonic::SEG_CS, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0x3F
    {.mnemonic = Mnemonic::AAS, .Destination = OperandCode::None, .Source = OperandCode::None},

    // 0x40
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

    // 0x48
    {.mnemonic = Mnemonic::DEC, .Destination = OperandCode::eAX, .Source = OperandCode::REX},
    // REX.WB
    {.mnemonic = Mnemonic::DEC, .Destination = OperandCode::eCX, .Source = OperandCode::REX},
    // REX.WX
    {.mnemonic = Mnemonic::DEC, .Destination = OperandCode::eDX, .Source = OperandCode::REX},
    // REX.WXB
    {.mnemonic = Mnemonic::DEC, .Destination = OperandCode::eBX, .Source = OperandCode::REX},
    // REX.WR
    {.mnemonic = Mnemonic::DEC, .Destination = OperandCode::eSP, .Source = OperandCode::REX},
    // REX.WRB
    {.mnemonic = Mnemonic::DEC, .Destination = OperandCode::eBP, .Source = OperandCode::REX},
    // REX.WRX
    {.mnemonic = Mnemonic::DEC, .Destination = OperandCode::eSI, .Source = OperandCode::REX},
    // REX.WRXB
    // 0x4F
    {.mnemonic = Mnemonic::DEC, .Destination = OperandCode::eDI, .Source = OperandCode::REX},

    // PUSH^d64 general register
    // 0x50
    {.mnemonic = Mnemonic::PUSH, .Destination = OperandCode::rAX, .Source = OperandCode::R8},
    {.mnemonic = Mnemonic::PUSH, .Destination = OperandCode::rCX, .Source = OperandCode::R9},
    {.mnemonic = Mnemonic::PUSH, .Destination = OperandCode::rDX, .Source = OperandCode::R10},
    {.mnemonic = Mnemonic::PUSH, .Destination = OperandCode::rBX, .Source = OperandCode::R11},
    {.mnemonic = Mnemonic::PUSH, .Destination = OperandCode::rSP, .Source = OperandCode::R12},
    {.mnemonic = Mnemonic::PUSH, .Destination = OperandCode::rBP, .Source = OperandCode::R13},
    {.mnemonic = Mnemonic::PUSH, .Destination = OperandCode::rSI, .Source = OperandCode::R14},
    // 0x57
    {.mnemonic = Mnemonic::PUSH, .Destination = OperandCode::rDI, .Source = OperandCode::R15},

    // PUSH^d64 general register
    // 0x58
    {.mnemonic = Mnemonic::POP, .Destination = OperandCode::rAX, .Source = OperandCode::R8},
    {.mnemonic = Mnemonic::POP, .Destination = OperandCode::rCX, .Source = OperandCode::R9},
    {.mnemonic = Mnemonic::POP, .Destination = OperandCode::rDX, .Source = OperandCode::R10},
    {.mnemonic = Mnemonic::POP, .Destination = OperandCode::rBX, .Source = OperandCode::R11},
    {.mnemonic = Mnemonic::POP, .Destination = OperandCode::rSP, .Source = OperandCode::R12},
    {.mnemonic = Mnemonic::POP, .Destination = OperandCode::rBP, .Source = OperandCode::R13},
    {.mnemonic = Mnemonic::POP, .Destination = OperandCode::rSI, .Source = OperandCode::R14},
    // 0x5F
    {.mnemonic = Mnemonic::POP, .Destination = OperandCode::rDI, .Source = OperandCode::R15},

    // 0x60
    {.mnemonic = Mnemonic::PUSHAD, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::POPAD, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::BOUND, .Destination = OperandCode::Gv, .Source = OperandCode::Ma, .ModRM = true, .RegField = ModRMRegMode::Register},
    // note only valid in amd64 in intel32 mode its ARPL i am focusing mainly on AMD64
    {.mnemonic = Mnemonic::MOVSXD, .Destination = OperandCode::Gv, .Source = OperandCode::Ev, .ModRM = true, .RegField = ModRMRegMode::Register},
    // SEG = FS (Prefix)
    {.mnemonic = Mnemonic::SEG_FS, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::SEG_GS, .Destination = OperandCode::None, .Source = OperandCode::None},
    // Operand/Address size (prefix)
    {.mnemonic = Mnemonic::OPERAND_SIZE_PREFIX, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0x67
    {.mnemonic = Mnemonic::ADDRESS_SIZE_PREFIX, .Destination = OperandCode::None, .Source = OperandCode::None},

    // 0x68
    {.mnemonic = Mnemonic::PUSH, .Destination = OperandCode::Iz, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::IMUL, .Destination = OperandCode::Gv, .Source = OperandCode::Ev, .Extra = OperandCode::Iz, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::PUSH, .Destination = OperandCode::Ib, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::IMUL, .Destination = OperandCode::Gv, .Source = OperandCode::Ev, .Extra = OperandCode::Ib, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::INS, .Destination = OperandCode::Yb, .Source = OperandCode::DX},
    {.mnemonic = Mnemonic::INS, .Destination = OperandCode::Yz, .Source = OperandCode::DX},
    {.mnemonic = Mnemonic::OUTS, .Destination = OperandCode::DX, .Source = OperandCode::Xb},
    // 0x6F
    {.mnemonic = Mnemonic::OUTS, .Destination = OperandCode::DX, .Source = OperandCode::Xz},

    // Jcc^f64, jb - short-displacement jump on condition
    // 0x70
    {.mnemonic = Mnemonic::JO, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::JNO, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::JB, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::JNB, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::JZ, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::JNZ, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::JBE, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    // 0x77
    {.mnemonic = Mnemonic::JA, .Destination = OperandCode::Jb, .Source = OperandCode::None},

    // 0x78
    {.mnemonic = Mnemonic::JS, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::JNS, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::JP, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::JNP, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::JL, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::JGE, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::JLE, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    // 0x7F
    {.mnemonic = Mnemonic::JG, .Destination = OperandCode::Jb, .Source = OperandCode::None},

    // for GROUP1 Bits 5, 4, and 3 of ModR/M byte used as an opcode extension
    // 0x80
    {.mnemonic = Mnemonic::GROUP1, .Destination = OperandCode::Eb, .Source = OperandCode::Ib, .ModRM = true, .RegField = ModRMRegMode::OpcodeExtension},
    {.mnemonic = Mnemonic::GROUP1, .Destination = OperandCode::Ev, .Source = OperandCode::Iz, .ModRM = true, .RegField = ModRMRegMode::OpcodeExtension},
    // invalid in amd64
    {.mnemonic = Mnemonic::GROUP1, .Destination = OperandCode::Eb, .Source = OperandCode::Ib, .ModRM = true, .RegField = ModRMRegMode::OpcodeExtension},
    {.mnemonic = Mnemonic::GROUP1, .Destination = OperandCode::Ev, .Source = OperandCode::Ib, .ModRM = true, .RegField = ModRMRegMode::OpcodeExtension},

    {.mnemonic = Mnemonic::TEST, .Destination = OperandCode::Eb, .Source = OperandCode::Gb, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::TEST, .Destination = OperandCode::Ev, .Source = OperandCode::Gv, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::XCHG, .Destination = OperandCode::Eb, .Source = OperandCode::Gb, .ModRM = true, .RegField = ModRMRegMode::Register},
    // 0x87
    {.mnemonic = Mnemonic::XCHG, .Destination = OperandCode::Ev, .Source = OperandCode::Gv, .ModRM = true, .RegField = ModRMRegMode::Register},

    // 0x88
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::Eb, .Source = OperandCode::Gb, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::Ev, .Source = OperandCode::Gv, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::Gb, .Source = OperandCode::Eb, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::Gv, .Source = OperandCode::Ev, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::Ev, .Source = OperandCode::Sw, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::LEA, .Destination = OperandCode::Gv, .Source = OperandCode::M, .ModRM = true, .RegField = ModRMRegMode::Register},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::Sw, .Source = OperandCode::Ew, .ModRM = true, .RegField = ModRMRegMode::Register},
    // 0x8F
    {.mnemonic = Mnemonic::GROUP1A, .Destination = OperandCode::Ev, .Source = OperandCode::None, .ModRM = true, .RegField = ModRMRegMode::OpcodeExtension},

    // XCHG word, double-word or quad-word register with rAX
    // 0x90
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

    // 0x98
    {.mnemonic = Mnemonic::CBW, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::CWD, .Destination = OperandCode::None, .Source = OperandCode::None},
    // far call
    {.mnemonic = Mnemonic::CALL, .Destination = OperandCode::Ap, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::WAIT, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::PUSHF, .Destination = OperandCode::Fv, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::SAHF, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0x9F
    {.mnemonic = Mnemonic::LAHF, .Destination = OperandCode::None, .Source = OperandCode::None},

    // 0XA0
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

    // 0xA8 (168)
    {.mnemonic = Mnemonic::TEST, .Destination = OperandCode::AL, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::TEST, .Destination = OperandCode::rAX, .Source = OperandCode::Iz},
    {.mnemonic = Mnemonic::STOS, .Destination = OperandCode::Yv, .Source = OperandCode::rAX},
    {.mnemonic = Mnemonic::LODS, .Destination = OperandCode::AL, .Source = OperandCode::Xb},
    {.mnemonic = Mnemonic::LODS, .Destination = OperandCode::rAX, .Source = OperandCode::Xv},
    {.mnemonic = Mnemonic::SCAS, .Destination = OperandCode::AL, .Source = OperandCode::Yb},
    // 0xAF
    {.mnemonic = Mnemonic::SCAS, .Destination = OperandCode::rAX, .Source = OperandCode::Yv},


    // MOV immediate byte into byte register
    // 0xB0
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::R8B, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::R9B, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::R10B, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::R11B, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::R12B, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::R13B, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::R14B, .Source = OperandCode::Ib},
    // 0xB7
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::R15B, .Source = OperandCode::Ib},

    // 0xB8 (MOV immediate word or double into word, double, or quad register)
    // swap with r8-r15 if REX.B is set
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::rAX, .Source = OperandCode::Iv},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::rCX, .Source = OperandCode::Iv},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::rDX, .Source = OperandCode::Iv},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::rBX, .Source = OperandCode::Iv},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::rSP, .Source = OperandCode::Iv},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::rBP, .Source = OperandCode::Iv},
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::rSI, .Source = OperandCode::Iv},
    // 0xBF
    {.mnemonic = Mnemonic::MOV, .Destination = OperandCode::rDI, .Source = OperandCode::Iv},

    // Shift Group2^1A, 1A = Bits 5, 4, and 3 of MODR/M byte used as an opcode extension
    // 0xC0
    {.mnemonic = Mnemonic::GROUP2, .Destination = OperandCode::Eb, .Source = OperandCode::Ib, .ModRM = true, .RegField = ModRMRegMode::OpcodeExtension},
    {.mnemonic = Mnemonic::GROUP2, .Destination = OperandCode::Ev, .Source = OperandCode::Ib, .ModRM = true, .RegField = ModRMRegMode::OpcodeExtension},

    // near RET^f64, f64 = operand size is forced to 64-bit bit operand size when in AMD64 mode
    {.mnemonic = Mnemonic::RET, .Destination = OperandCode::Iw, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::RET, .Destination = OperandCode::None, .Source = OperandCode::None},

    {.mnemonic = Mnemonic::VEX2, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::VEX1, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::GROUP11, .Destination = OperandCode::Eb, .Source = OperandCode::Ib, .ModRM = true, .RegField = ModRMRegMode::OpcodeExtension},
    // 0xC7
    {.mnemonic = Mnemonic::GROUP11, .Destination = OperandCode::Ev, .Source = OperandCode::Iz, .ModRM = true, .RegField = ModRMRegMode::OpcodeExtension},

    // 0xC8
    {.mnemonic = Mnemonic::ENTER, .Destination = OperandCode::Iw, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::LEAVE, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::RETF, .Destination = OperandCode::Iw, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::RETF, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::INT3, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::INT, .Destination = OperandCode::Ib, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::INVALID, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xCF
    {.mnemonic = Mnemonic::IRET, .Destination = OperandCode::None, .Source = OperandCode::None},

    // 0xD0
    {.mnemonic = Mnemonic::GROUP2, .Destination = OperandCode::Eb, .Source = OperandCode::One, .ModRM = true, .RegField = ModRMRegMode::OpcodeExtension},
    {.mnemonic = Mnemonic::GROUP2, .Destination = OperandCode::Ev, .Source = OperandCode::One, .ModRM = true, .RegField = ModRMRegMode::OpcodeExtension},
    {.mnemonic = Mnemonic::GROUP2, .Destination = OperandCode::Eb, .Source = OperandCode::CL, .ModRM = true, .RegField = ModRMRegMode::OpcodeExtension},
    {.mnemonic = Mnemonic::GROUP2, .Destination = OperandCode::Ev, .Source = OperandCode::CL, .ModRM = true, .RegField = ModRMRegMode::OpcodeExtension},
    {.mnemonic = Mnemonic::INVALID, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::INVALID, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::INVALID, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xD7
    {.mnemonic = Mnemonic::XLAT, .Destination = OperandCode::None, .Source = OperandCode::None},

    {.mnemonic = Mnemonic::X87, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::X87, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::X87, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::X87, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::X87, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::X87, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::X87, .Destination = OperandCode::None, .Source = OperandCode::None},
    // 0xDF
    {.mnemonic = Mnemonic::X87, .Destination = OperandCode::None, .Source = OperandCode::None},

    // 0xE0
    {.mnemonic = Mnemonic::LOOPNE, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::LOOPE, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::LOOP, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::JRCXZ, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::IN, .Destination = OperandCode::AL, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::IN, .Destination = OperandCode::eAX, .Source = OperandCode::Ib},
    {.mnemonic = Mnemonic::OUT, .Destination = OperandCode::Ib, .Source = OperandCode::AL},
    // 0xE7
    {.mnemonic = Mnemonic::OUT, .Destination = OperandCode::Ib, .Source = OperandCode::eAX},

    // 0xE8
    {.mnemonic = Mnemonic::CALL, .Destination = OperandCode::Jz, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::JMP, .Destination = OperandCode::Jz, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::INVALID, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::JMP, .Destination = OperandCode::Jb, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::IN, .Destination = OperandCode::AL, .Source = OperandCode::DX},
    {.mnemonic = Mnemonic::IN, .Destination = OperandCode::eAX, .Source = OperandCode::DX},
    {.mnemonic = Mnemonic::OUT, .Destination = OperandCode::DX, .Source = OperandCode::AL},
    // 0xEF
    {.mnemonic = Mnemonic::OUT, .Destination = OperandCode::DX, .Source = OperandCode::eAX},

    // 0xF0
    {.mnemonic = Mnemonic::LOCK, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::INT1, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::REPNE, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::REP, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::HLT, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::CMC, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::GROUP3, .Destination = OperandCode::Eb, .Source = OperandCode::None, .ModRM = true, .RegField = ModRMRegMode::OpcodeExtension},
    // 0xF7
    {.mnemonic = Mnemonic::GROUP3, .Destination = OperandCode::Ev, .Source = OperandCode::None, .ModRM = true, .RegField = ModRMRegMode::OpcodeExtension},

    // 0xF8
    {.mnemonic = Mnemonic::CLC, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::STC, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::CLI, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::STI, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::CLD, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::STD, .Destination = OperandCode::None, .Source = OperandCode::None},
    {.mnemonic = Mnemonic::GROUP4, .Destination = OperandCode::Eb, .Source = OperandCode::None, .ModRM = true, .RegField = ModRMRegMode::OpcodeExtension},
    // 0xFF
    {.mnemonic = Mnemonic::GROUP5, .Destination = OperandCode::Ev, .Source = OperandCode::None, .ModRM = true, .RegField = ModRMRegMode::OpcodeExtension},
};

//constexpr bool RequiresModRM(OperandCode Code) noexcept
//{
//  return Code == OperandCode::Eb || Code == OperandCode::Ev || Code == OperandCode::Ew || Code == OperandCode::Gb
//         || Code == OperandCode::Gv || Code == OperandCode::GS || Code == OperandCode::M || Code == OperandCode::Q
//         || Code == OperandCode::W || Code == OperandCode::N || Code == OperandCode::Sw;
//}
