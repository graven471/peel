#pragma once

#include "Opcodes.hpp"

enum class mod_rmRegMode : uint8_t
{
  None,
  Register,        // /r
  OpcodeExtension  // /0 through /7
};

// Mnemonic dst, src
struct InstructionDesc
{
  Mnemonic      mnemonic;
  OperandCode   destination;
  OperandCode   source;
  OperandCode   extra     = OperandCode::None;
  bool          mod_rm    = false;
  mod_rmRegMode reg_field = mod_rmRegMode::None;
  std::uint8_t  reg_extension;
};

// E, G, C, M, D, M, Q, R, S, U, V, W needs ModR/M
// E  -> ModR/M.r/m
// G  -> ModR/M.reg
// C  -> ModR/M.reg (control register)
// D  -> ModR/M.reg (debug register)
// b/v/w/d/q/etc. -> describe the operand's size/type
static constexpr InstructionDesc OPCODE_TABLE[256] = {
    // 0x00
    {.mnemonic = Mnemonic::ADD, .destination = OperandCode::Eb, .source = OperandCode::Gb, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::ADD, .destination = OperandCode::Ev, .source = OperandCode::Gv, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::ADD, .destination = OperandCode::Gb, .source = OperandCode::Eb, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::ADD, .destination = OperandCode::Gv, .source = OperandCode::Ev, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::ADD, .destination = OperandCode::AL, .source = OperandCode::Ib},
    {.mnemonic = Mnemonic::ADD, .destination = OperandCode::rAX, .source = OperandCode::Iz},
    {.mnemonic = Mnemonic::PUSH, .destination = OperandCode::Es, .source = OperandCode::None},
    // 0x7
    {.mnemonic = Mnemonic::POP, .destination = OperandCode::Es, .source = OperandCode::None},

    // 0x08
    {.mnemonic = Mnemonic::OR, .destination = OperandCode::Eb, .source = OperandCode::Gb, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::OR, .destination = OperandCode::Ev, .source = OperandCode::Gv, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::OR, .destination = OperandCode::Gb, .source = OperandCode::Eb, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::OR, .destination = OperandCode::Gv, .source = OperandCode::Ev, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::OR, .destination = OperandCode::AL, .source = OperandCode::Ib},
    {.mnemonic = Mnemonic::OR, .destination = OperandCode::rAX, .source = OperandCode::Iz},
    {.mnemonic = Mnemonic::PUSH, .destination = OperandCode::CS, .source = OperandCode::None},
    // 0x0F
    {.mnemonic = Mnemonic::ESCAPE_2BYTE, .destination = OperandCode::None, .source = OperandCode::None},

    // 0x10
    {.mnemonic = Mnemonic::ADC, .destination = OperandCode::Eb, .source = OperandCode::Gb, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::ADC, .destination = OperandCode::Ev, .source = OperandCode::Gv, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::ADC, .destination = OperandCode::Gb, .source = OperandCode::Eb, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::ADC, .destination = OperandCode::Gv, .source = OperandCode::Ev, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::ADC, .destination = OperandCode::AL, .source = OperandCode::Ib},
    {.mnemonic = Mnemonic::ADC, .destination = OperandCode::rAX, .source = OperandCode::Iz},
    {.mnemonic = Mnemonic::PUSH, .destination = OperandCode::SS, .source = OperandCode::None},
    // 0x17
    {.mnemonic = Mnemonic::POP, .destination = OperandCode::SS, .source = OperandCode::None},

    // 0x18
    {.mnemonic = Mnemonic::SBB, .destination = OperandCode::Eb, .source = OperandCode::Gb, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::SBB, .destination = OperandCode::Ev, .source = OperandCode::Gv, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::SBB, .destination = OperandCode::Gb, .source = OperandCode::Eb, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::SBB, .destination = OperandCode::Gv, .source = OperandCode::Ev, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::SBB, .destination = OperandCode::AL, .source = OperandCode::Ib},
    {.mnemonic = Mnemonic::SBB, .destination = OperandCode::rAX, .source = OperandCode::Iz},
    // only valid in amd64 mode
    {.mnemonic = Mnemonic::PUSH, .destination = OperandCode::DS, .source = OperandCode::None},
    // 0xF
    {.mnemonic = Mnemonic::POP, .destination = OperandCode::DS, .source = OperandCode::None},

    // 0x20
    {.mnemonic = Mnemonic::AND, .destination = OperandCode::Eb, .source = OperandCode::Gb, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::AND, .destination = OperandCode::Ev, .source = OperandCode::Gv, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::AND, .destination = OperandCode::Gb, .source = OperandCode::Eb, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::AND, .destination = OperandCode::Gv, .source = OperandCode::Ev, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::AND, .destination = OperandCode::AL, .source = OperandCode::Ib},
    {.mnemonic = Mnemonic::AND, .destination = OperandCode::rAX, .source = OperandCode::Iz},
    {.mnemonic = Mnemonic::SEG_ES, .destination = OperandCode::None, .source = OperandCode::None},
    // 0x27
    {.mnemonic = Mnemonic::DAA, .destination = OperandCode::None, .source = OperandCode::None},

    // 0x28
    {.mnemonic = Mnemonic::SUB, .destination = OperandCode::Eb, .source = OperandCode::Gb, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::SUB, .destination = OperandCode::Ev, .source = OperandCode::Gv, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::SUB, .destination = OperandCode::Gb, .source = OperandCode::Eb, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::SUB, .destination = OperandCode::Gv, .source = OperandCode::Ev, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::SUB, .destination = OperandCode::AL, .source = OperandCode::Ib},
    {.mnemonic = Mnemonic::SUB, .destination = OperandCode::rAX, .source = OperandCode::Iz},
    {.mnemonic = Mnemonic::SEG_CS, .destination = OperandCode::None, .source = OperandCode::None},
    // only valid in amd64
    // 0x2F
    {.mnemonic = Mnemonic::DAS, .destination = OperandCode::None, .source = OperandCode::None},

    // 0x30
    {.mnemonic = Mnemonic::XOR, .destination = OperandCode::Eb, .source = OperandCode::Gb, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::XOR, .destination = OperandCode::Ev, .source = OperandCode::Gv, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::XOR, .destination = OperandCode::Gb, .source = OperandCode::Eb, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::XOR, .destination = OperandCode::Gv, .source = OperandCode::Ev, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::XOR, .destination = OperandCode::AL, .source = OperandCode::Ib},
    {.mnemonic = Mnemonic::XOR, .destination = OperandCode::rAX, .source = OperandCode::Iz},
    {.mnemonic = Mnemonic::SEG_SS, .destination = OperandCode::None, .source = OperandCode::None},
    // 0x37
    {.mnemonic = Mnemonic::AAA, .destination = OperandCode::None, .source = OperandCode::None},

    // 0x38
    {.mnemonic = Mnemonic::CMP, .destination = OperandCode::Eb, .source = OperandCode::Gb, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::CMP, .destination = OperandCode::Ev, .source = OperandCode::Gv, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::CMP, .destination = OperandCode::Gb, .source = OperandCode::Eb, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::CMP, .destination = OperandCode::Gv, .source = OperandCode::Ev, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::CMP, .destination = OperandCode::AL, .source = OperandCode::Ib},
    {.mnemonic = Mnemonic::CMP, .destination = OperandCode::rAX, .source = OperandCode::Iz},
    {.mnemonic = Mnemonic::SEG_CS, .destination = OperandCode::None, .source = OperandCode::None},
    // 0x3F
    {.mnemonic = Mnemonic::AAS, .destination = OperandCode::None, .source = OperandCode::None},

    // 0x40
    // INC^i64 general register / REX^o64 prefixes only valid in amd64 mode
    {.mnemonic = Mnemonic::INC, .destination = OperandCode::eAX, .source = OperandCode::REX},
    // REX.B
    {.mnemonic = Mnemonic::INC, .destination = OperandCode::eCX, .source = OperandCode::REX},
    // REX.X
    {.mnemonic = Mnemonic::INC, .destination = OperandCode::eDX, .source = OperandCode::REX},
    // REX.XB
    {.mnemonic = Mnemonic::INC, .destination = OperandCode::eBX, .source = OperandCode::REX},
    // REX.R
    {.mnemonic = Mnemonic::INC, .destination = OperandCode::eSP, .source = OperandCode::REX},
    // REX.RB
    {.mnemonic = Mnemonic::INC, .destination = OperandCode::eBP, .source = OperandCode::REX},
    // REX.RX
    {.mnemonic = Mnemonic::INC, .destination = OperandCode::eSI, .source = OperandCode::REX},
    // REX.RXB
    // 0x47
    {.mnemonic = Mnemonic::INC, .destination = OperandCode::eDI, .source = OperandCode::REX},

    // 0x48
    {.mnemonic = Mnemonic::DEC, .destination = OperandCode::eAX, .source = OperandCode::REX},
    // REX.WB
    {.mnemonic = Mnemonic::DEC, .destination = OperandCode::eCX, .source = OperandCode::REX},
    // REX.WX
    {.mnemonic = Mnemonic::DEC, .destination = OperandCode::eDX, .source = OperandCode::REX},
    // REX.WXB
    {.mnemonic = Mnemonic::DEC, .destination = OperandCode::eBX, .source = OperandCode::REX},
    // REX.WR
    {.mnemonic = Mnemonic::DEC, .destination = OperandCode::eSP, .source = OperandCode::REX},
    // REX.WRB
    {.mnemonic = Mnemonic::DEC, .destination = OperandCode::eBP, .source = OperandCode::REX},
    // REX.WRX
    {.mnemonic = Mnemonic::DEC, .destination = OperandCode::eSI, .source = OperandCode::REX},
    // REX.WRXB
    // 0x4F
    {.mnemonic = Mnemonic::DEC, .destination = OperandCode::eDI, .source = OperandCode::REX},

    // PUSH^d64 general register
    // 0x50
    {.mnemonic = Mnemonic::PUSH, .destination = OperandCode::rAX, .source = OperandCode::R8},
    {.mnemonic = Mnemonic::PUSH, .destination = OperandCode::rCX, .source = OperandCode::R9},
    {.mnemonic = Mnemonic::PUSH, .destination = OperandCode::rDX, .source = OperandCode::R10},
    {.mnemonic = Mnemonic::PUSH, .destination = OperandCode::rBX, .source = OperandCode::R11},
    {.mnemonic = Mnemonic::PUSH, .destination = OperandCode::rSP, .source = OperandCode::R12},
    {.mnemonic = Mnemonic::PUSH, .destination = OperandCode::rBP, .source = OperandCode::R13},
    {.mnemonic = Mnemonic::PUSH, .destination = OperandCode::rSI, .source = OperandCode::R14},
    // 0x57
    {.mnemonic = Mnemonic::PUSH, .destination = OperandCode::rDI, .source = OperandCode::R15},

    // PUSH^d64 general register
    // 0x58
    {.mnemonic = Mnemonic::POP, .destination = OperandCode::rAX, .source = OperandCode::R8},
    {.mnemonic = Mnemonic::POP, .destination = OperandCode::rCX, .source = OperandCode::R9},
    {.mnemonic = Mnemonic::POP, .destination = OperandCode::rDX, .source = OperandCode::R10},
    {.mnemonic = Mnemonic::POP, .destination = OperandCode::rBX, .source = OperandCode::R11},
    {.mnemonic = Mnemonic::POP, .destination = OperandCode::rSP, .source = OperandCode::R12},
    {.mnemonic = Mnemonic::POP, .destination = OperandCode::rBP, .source = OperandCode::R13},
    {.mnemonic = Mnemonic::POP, .destination = OperandCode::rSI, .source = OperandCode::R14},
    // 0x5F
    {.mnemonic = Mnemonic::POP, .destination = OperandCode::rDI, .source = OperandCode::R15},

    // 0x60
    {.mnemonic = Mnemonic::PUSHAD, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::POPAD, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::BOUND, .destination = OperandCode::Gv, .source = OperandCode::Ma, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    // note only valid in amd64 in intel32 mode its ARPL i am focusing mainly on AMD64
    {.mnemonic = Mnemonic::MOVSXD, .destination = OperandCode::Gv, .source = OperandCode::Ev, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    // SEG = FS (Prefix)
    {.mnemonic = Mnemonic::SEG_FS, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::SEG_GS, .destination = OperandCode::None, .source = OperandCode::None},
    // Operand/Address size (prefix)
    {.mnemonic = Mnemonic::OPERAND_SIZE_PREFIX, .destination = OperandCode::None, .source = OperandCode::None},
    // 0x67
    {.mnemonic = Mnemonic::ADDRESS_SIZE_PREFIX, .destination = OperandCode::None, .source = OperandCode::None},

    // 0x68
    {.mnemonic = Mnemonic::PUSH, .destination = OperandCode::Iz, .source = OperandCode::None},
    {.mnemonic    = Mnemonic::IMUL,
     .destination = OperandCode::Gv,
     .source      = OperandCode::Ev,
     .extra       = OperandCode::Iz,
     .mod_rm      = true,
     .reg_field   = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::PUSH, .destination = OperandCode::Ib, .source = OperandCode::None},
    {.mnemonic    = Mnemonic::IMUL,
     .destination = OperandCode::Gv,
     .source      = OperandCode::Ev,
     .extra       = OperandCode::Ib,
     .mod_rm      = true,
     .reg_field   = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::INS, .destination = OperandCode::Yb, .source = OperandCode::DX},
    {.mnemonic = Mnemonic::INS, .destination = OperandCode::Yz, .source = OperandCode::DX},
    {.mnemonic = Mnemonic::OUTS, .destination = OperandCode::DX, .source = OperandCode::Xb},
    // 0x6F
    {.mnemonic = Mnemonic::OUTS, .destination = OperandCode::DX, .source = OperandCode::Xz},

    // Jcc^f64, jb - short-displacement jump on condition
    // 0x70
    {.mnemonic = Mnemonic::JO, .destination = OperandCode::Jb, .source = OperandCode::None},
    {.mnemonic = Mnemonic::JNO, .destination = OperandCode::Jb, .source = OperandCode::None},
    {.mnemonic = Mnemonic::JB, .destination = OperandCode::Jb, .source = OperandCode::None},
    {.mnemonic = Mnemonic::JNB, .destination = OperandCode::Jb, .source = OperandCode::None},
    {.mnemonic = Mnemonic::JZ, .destination = OperandCode::Jb, .source = OperandCode::None},
    {.mnemonic = Mnemonic::JNZ, .destination = OperandCode::Jb, .source = OperandCode::None},
    {.mnemonic = Mnemonic::JBE, .destination = OperandCode::Jb, .source = OperandCode::None},
    // 0x77
    {.mnemonic = Mnemonic::JA, .destination = OperandCode::Jb, .source = OperandCode::None},

    // 0x78
    {.mnemonic = Mnemonic::JS, .destination = OperandCode::Jb, .source = OperandCode::None},
    {.mnemonic = Mnemonic::JNS, .destination = OperandCode::Jb, .source = OperandCode::None},
    {.mnemonic = Mnemonic::JP, .destination = OperandCode::Jb, .source = OperandCode::None},
    {.mnemonic = Mnemonic::JNP, .destination = OperandCode::Jb, .source = OperandCode::None},
    {.mnemonic = Mnemonic::JL, .destination = OperandCode::Jb, .source = OperandCode::None},
    {.mnemonic = Mnemonic::JGE, .destination = OperandCode::Jb, .source = OperandCode::None},
    {.mnemonic = Mnemonic::JLE, .destination = OperandCode::Jb, .source = OperandCode::None},
    // 0x7F
    {.mnemonic = Mnemonic::JG, .destination = OperandCode::Jb, .source = OperandCode::None},

    // for GROUP1 Bits 5, 4, and 3 of ModR/M byte used as an opcode extension
    // 0x80
    {.mnemonic = Mnemonic::GROUP1, .destination = OperandCode::Eb, .source = OperandCode::Ib, .mod_rm = true, .reg_field = mod_rmRegMode::OpcodeExtension},
    {.mnemonic = Mnemonic::GROUP1, .destination = OperandCode::Ev, .source = OperandCode::Iz, .mod_rm = true, .reg_field = mod_rmRegMode::OpcodeExtension},
    // invalid in amd64
    {.mnemonic = Mnemonic::GROUP1, .destination = OperandCode::Eb, .source = OperandCode::Ib, .mod_rm = true, .reg_field = mod_rmRegMode::OpcodeExtension},
    {.mnemonic = Mnemonic::GROUP1, .destination = OperandCode::Ev, .source = OperandCode::Ib, .mod_rm = true, .reg_field = mod_rmRegMode::OpcodeExtension},

    {.mnemonic = Mnemonic::TEST, .destination = OperandCode::Eb, .source = OperandCode::Gb, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::TEST, .destination = OperandCode::Ev, .source = OperandCode::Gv, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::XCHG, .destination = OperandCode::Eb, .source = OperandCode::Gb, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    // 0x87
    {.mnemonic = Mnemonic::XCHG, .destination = OperandCode::Ev, .source = OperandCode::Gv, .mod_rm = true, .reg_field = mod_rmRegMode::Register},

    // 0x88
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::Eb, .source = OperandCode::Gb, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::Ev, .source = OperandCode::Gv, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::Gb, .source = OperandCode::Eb, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::Gv, .source = OperandCode::Ev, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::Ev, .source = OperandCode::Sw, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::LEA, .destination = OperandCode::Gv, .source = OperandCode::M, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::Sw, .source = OperandCode::Ew, .mod_rm = true, .reg_field = mod_rmRegMode::Register},
    // 0x8F
    {.mnemonic = Mnemonic::GROUP1A, .destination = OperandCode::Ev, .source = OperandCode::None, .mod_rm = true, .reg_field = mod_rmRegMode::OpcodeExtension},

    // XCHG word, double-word or quad-word register with rAX
    // 0x90
    // this can be either NOP or PAUSE(F3) or XCHG let prefix scanner resolve it
    {.mnemonic = Mnemonic::NOP, .destination = OperandCode::R8, .source = OperandCode::rAX},
    {.mnemonic = Mnemonic::XCHG, .destination = OperandCode::rCX, .source = OperandCode::R9},
    {.mnemonic = Mnemonic::XCHG, .destination = OperandCode::rDX, .source = OperandCode::R10},
    {.mnemonic = Mnemonic::XCHG, .destination = OperandCode::rBX, .source = OperandCode::R11},
    {.mnemonic = Mnemonic::XCHG, .destination = OperandCode::rSP, .source = OperandCode::R12},
    {.mnemonic = Mnemonic::XCHG, .destination = OperandCode::rBP, .source = OperandCode::R13},
    {.mnemonic = Mnemonic::XCHG, .destination = OperandCode::rSI, .source = OperandCode::R14},
    // 0x97
    {.mnemonic = Mnemonic::XCHG, .destination = OperandCode::rDI, .source = OperandCode::R15},

    // 0x98
    {.mnemonic = Mnemonic::CBW, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::CWD, .destination = OperandCode::None, .source = OperandCode::None},
    // far call
    {.mnemonic = Mnemonic::CALL, .destination = OperandCode::Ap, .source = OperandCode::None},
    {.mnemonic = Mnemonic::WAIT, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::PUSHF, .destination = OperandCode::Fv, .source = OperandCode::None},
    {.mnemonic = Mnemonic::SAHF, .destination = OperandCode::None, .source = OperandCode::None},
    // 0x9F
    {.mnemonic = Mnemonic::LAHF, .destination = OperandCode::None, .source = OperandCode::None},

    // 0XA0
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::AL, .source = OperandCode::Ob},
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::rAX, .source = OperandCode::Ov},
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::Ob, .source = OperandCode::AL},
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::Ov, .source = OperandCode::rAX},
    // MOVS/B
    {.mnemonic = Mnemonic::MOVS, .destination = OperandCode::Yb, .source = OperandCode::Xb},
    // MOVS/W/D/Q
    {.mnemonic = Mnemonic::MOVS, .destination = OperandCode::Yv, .source = OperandCode::Xv},
    // CMPS/B
    {.mnemonic = Mnemonic::CMPS, .destination = OperandCode::Yb, .source = OperandCode::Xb},
    // CMPS/W/D/Q
    // 0xA7
    {.mnemonic = Mnemonic::CMPS, .destination = OperandCode::Yv, .source = OperandCode::Xv},

    // 0xA8 (168)
    {.mnemonic = Mnemonic::TEST, .destination = OperandCode::AL, .source = OperandCode::Ib},
    {.mnemonic = Mnemonic::TEST, .destination = OperandCode::rAX, .source = OperandCode::Iz},
    {.mnemonic = Mnemonic::STOS, .destination = OperandCode::Yv, .source = OperandCode::rAX},
    {.mnemonic = Mnemonic::LODS, .destination = OperandCode::AL, .source = OperandCode::Xb},
    {.mnemonic = Mnemonic::LODS, .destination = OperandCode::rAX, .source = OperandCode::Xv},
    {.mnemonic = Mnemonic::SCAS, .destination = OperandCode::AL, .source = OperandCode::Yb},
    // 0xAF
    {.mnemonic = Mnemonic::SCAS, .destination = OperandCode::rAX, .source = OperandCode::Yv},


    // MOV immediate byte into byte register
    // 0xB0
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::R8B, .source = OperandCode::Ib},
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::R9B, .source = OperandCode::Ib},
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::R10B, .source = OperandCode::Ib},
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::R11B, .source = OperandCode::Ib},
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::R12B, .source = OperandCode::Ib},
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::R13B, .source = OperandCode::Ib},
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::R14B, .source = OperandCode::Ib},
    // 0xB7
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::R15B, .source = OperandCode::Ib},

    // 0xB8 (MOV immediate word or double into word, double, or quad register)
    // swap with r8-r15 if REX.B is set
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::rAX, .source = OperandCode::Iv},
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::rCX, .source = OperandCode::Iv},
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::rDX, .source = OperandCode::Iv},
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::rBX, .source = OperandCode::Iv},
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::rSP, .source = OperandCode::Iv},
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::rBP, .source = OperandCode::Iv},
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::rSI, .source = OperandCode::Iv},
    // 0xBF
    {.mnemonic = Mnemonic::MOV, .destination = OperandCode::rDI, .source = OperandCode::Iv},

    // Shift Group2^1A, 1A = Bits 5, 4, and 3 of MODR/M byte used as an opcode extension
    // 0xC0
    {.mnemonic = Mnemonic::GROUP2, .destination = OperandCode::Eb, .source = OperandCode::Ib, .mod_rm = true, .reg_field = mod_rmRegMode::OpcodeExtension},
    {.mnemonic = Mnemonic::GROUP2, .destination = OperandCode::Ev, .source = OperandCode::Ib, .mod_rm = true, .reg_field = mod_rmRegMode::OpcodeExtension},

    // near RET^f64, f64 = operand size is forced to 64-bit bit operand size when in AMD64 mode
    {.mnemonic = Mnemonic::RET, .destination = OperandCode::Iw, .source = OperandCode::None},
    {.mnemonic = Mnemonic::RET, .destination = OperandCode::None, .source = OperandCode::None},

    {.mnemonic = Mnemonic::VEX2, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::VEX1, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::GROUP11, .destination = OperandCode::Eb, .source = OperandCode::Ib, .mod_rm = true, .reg_field = mod_rmRegMode::OpcodeExtension},
    // 0xC7
    {.mnemonic = Mnemonic::GROUP11, .destination = OperandCode::Ev, .source = OperandCode::Iz, .mod_rm = true, .reg_field = mod_rmRegMode::OpcodeExtension},

    // 0xC8
    {.mnemonic = Mnemonic::ENTER, .destination = OperandCode::Iw, .source = OperandCode::Ib},
    {.mnemonic = Mnemonic::LEAVE, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::RETF, .destination = OperandCode::Iw, .source = OperandCode::None},
    {.mnemonic = Mnemonic::RETF, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::INT3, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::INT, .destination = OperandCode::Ib, .source = OperandCode::None},
    {.mnemonic = Mnemonic::INVALID, .destination = OperandCode::None, .source = OperandCode::None},
    // 0xCF
    {.mnemonic = Mnemonic::IRET, .destination = OperandCode::None, .source = OperandCode::None},

    // 0xD0
    {.mnemonic = Mnemonic::GROUP2, .destination = OperandCode::Eb, .source = OperandCode::One, .mod_rm = true, .reg_field = mod_rmRegMode::OpcodeExtension},
    {.mnemonic = Mnemonic::GROUP2, .destination = OperandCode::Ev, .source = OperandCode::One, .mod_rm = true, .reg_field = mod_rmRegMode::OpcodeExtension},
    {.mnemonic = Mnemonic::GROUP2, .destination = OperandCode::Eb, .source = OperandCode::CL, .mod_rm = true, .reg_field = mod_rmRegMode::OpcodeExtension},
    {.mnemonic = Mnemonic::GROUP2, .destination = OperandCode::Ev, .source = OperandCode::CL, .mod_rm = true, .reg_field = mod_rmRegMode::OpcodeExtension},
    {.mnemonic = Mnemonic::INVALID, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::INVALID, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::INVALID, .destination = OperandCode::None, .source = OperandCode::None},
    // 0xD7
    {.mnemonic = Mnemonic::XLAT, .destination = OperandCode::None, .source = OperandCode::None},

    {.mnemonic = Mnemonic::X87, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::X87, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::X87, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::X87, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::X87, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::X87, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::X87, .destination = OperandCode::None, .source = OperandCode::None},
    // 0xDF
    {.mnemonic = Mnemonic::X87, .destination = OperandCode::None, .source = OperandCode::None},

    // 0xE0
    {.mnemonic = Mnemonic::LOOPNE, .destination = OperandCode::Jb, .source = OperandCode::None},
    {.mnemonic = Mnemonic::LOOPE, .destination = OperandCode::Jb, .source = OperandCode::None},
    {.mnemonic = Mnemonic::LOOP, .destination = OperandCode::Jb, .source = OperandCode::None},
    {.mnemonic = Mnemonic::JRCXZ, .destination = OperandCode::Jb, .source = OperandCode::None},
    {.mnemonic = Mnemonic::IN, .destination = OperandCode::AL, .source = OperandCode::Ib},
    {.mnemonic = Mnemonic::IN, .destination = OperandCode::eAX, .source = OperandCode::Ib},
    {.mnemonic = Mnemonic::OUT, .destination = OperandCode::Ib, .source = OperandCode::AL},
    // 0xE7
    {.mnemonic = Mnemonic::OUT, .destination = OperandCode::Ib, .source = OperandCode::eAX},

    // 0xE8
    {.mnemonic = Mnemonic::CALL, .destination = OperandCode::Jz, .source = OperandCode::None},
    {.mnemonic = Mnemonic::JMP, .destination = OperandCode::Jz, .source = OperandCode::None},
    {.mnemonic = Mnemonic::INVALID, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::JMP, .destination = OperandCode::Jb, .source = OperandCode::None},
    {.mnemonic = Mnemonic::IN, .destination = OperandCode::AL, .source = OperandCode::DX},
    {.mnemonic = Mnemonic::IN, .destination = OperandCode::eAX, .source = OperandCode::DX},
    {.mnemonic = Mnemonic::OUT, .destination = OperandCode::DX, .source = OperandCode::AL},
    // 0xEF
    {.mnemonic = Mnemonic::OUT, .destination = OperandCode::DX, .source = OperandCode::eAX},

    // 0xF0
    {.mnemonic = Mnemonic::LOCK, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::INT1, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::REPNE, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::REP, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::HLT, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::CMC, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::GROUP3, .destination = OperandCode::Eb, .source = OperandCode::None, .mod_rm = true, .reg_field = mod_rmRegMode::OpcodeExtension},
    // 0xF7
    {.mnemonic = Mnemonic::GROUP3, .destination = OperandCode::Ev, .source = OperandCode::None, .mod_rm = true, .reg_field = mod_rmRegMode::OpcodeExtension},

    // 0xF8
    {.mnemonic = Mnemonic::CLC, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::STC, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::CLI, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::STI, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::CLD, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::STD, .destination = OperandCode::None, .source = OperandCode::None},
    {.mnemonic = Mnemonic::GROUP4, .destination = OperandCode::Eb, .source = OperandCode::None, .mod_rm = true, .reg_field = mod_rmRegMode::OpcodeExtension},
    // 0xFF
    {.mnemonic = Mnemonic::GROUP5, .destination = OperandCode::Ev, .source = OperandCode::None, .mod_rm = true, .reg_field = mod_rmRegMode::OpcodeExtension},
};

//constexpr bool Requiresmod_rm(OperandCode Code) noexcept
//{
//  return Code == OperandCode::Eb || Code == OperandCode::Ev || Code == OperandCode::Ew || Code == OperandCode::Gb
//         || Code == OperandCode::Gv || Code == OperandCode::GS || Code == OperandCode::M || Code == OperandCode::Q
//         || Code == OperandCode::W || Code == OperandCode::N || Code == OperandCode::Sw;
//}
