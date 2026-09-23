#pragma once

#include <cstddef>

enum class Mnemonic : std::uint16_t
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