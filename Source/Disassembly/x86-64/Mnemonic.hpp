#pragma once

#include <cstddef>
#include <string_view>

// TODO: replace this with c++ 26 static reflection when msvc adds support for it

#define MNEMONIC_LIST(X)                                                                                               \
  X(INVALID)                                                                                                           \
  X(MOV)                                                                                                               \
  X(MOVS)                                                                                                              \
  X(MOVSXD)                                                                                                            \
  X(MOVBE)                                                                                                             \
  X(ESCAPE_2BYTE)                                                                                                      \
  X(LEA)                                                                                                               \
  X(RET)                                                                                                               \
  X(RETF)                                                                                                              \
  X(ADD)                                                                                                               \
  X(SUB)                                                                                                               \
  X(IMUL)                                                                                                              \
  X(CBW)                                                                                                               \
  X(CWD)                                                                                                               \
  X(INS)                                                                                                               \
  X(ADC)                                                                                                               \
  X(OR)                                                                                                                \
  X(AND)                                                                                                               \
  X(XOR)                                                                                                               \
  X(INC)                                                                                                               \
  X(DEC)                                                                                                               \
  X(CMP)                                                                                                               \
  X(PUSH)                                                                                                              \
  X(PUSHF)                                                                                                             \
  X(PUSHAD)                                                                                                            \
  X(POP)                                                                                                               \
  X(POPF)                                                                                                              \
  X(POPAD)                                                                                                             \
  X(TEST)                                                                                                              \
  X(INT)                                                                                                               \
  X(INT1)                                                                                                              \
  X(INT3)                                                                                                              \
  X(INTO)                                                                                                              \
  X(IRET)                                                                                                              \
  X(IN)                                                                                                                \
  X(OUT)                                                                                                               \
  X(OUTS)                                                                                                              \
  X(JO)                                                                                                                \
  X(JNO)                                                                                                               \
  X(JB)                                                                                                                \
  X(JNB)                                                                                                               \
  X(JZ)                                                                                                                \
  X(JNZ)                                                                                                               \
  X(JBE)                                                                                                               \
  X(JA)                                                                                                                \
  X(JS)                                                                                                                \
  X(JNS)                                                                                                               \
  X(JP)                                                                                                                \
  X(JNP)                                                                                                               \
  X(JL)                                                                                                                \
  X(JGE)                                                                                                               \
  X(JLE)                                                                                                               \
  X(JG)                                                                                                                \
  X(JMP)                                                                                                               \
  X(JRCXZ)                                                                                                             \
  X(CALL)                                                                                                              \
  X(WAIT)                                                                                                              \
  X(BOUND)                                                                                                             \
  X(XCHG)                                                                                                              \
  X(XLAT)                                                                                                              \
  X(NOP)                                                                                                               \
  X(HLT)                                                                                                               \
  X(CMC)                                                                                                               \
  X(CLC)                                                                                                               \
  X(STC)                                                                                                               \
  X(CLI)                                                                                                               \
  X(STI)                                                                                                               \
  X(CLD)                                                                                                               \
  X(STD)                                                                                                               \
  X(CMPS)                                                                                                              \
  X(LODS)                                                                                                              \
  X(STOS)                                                                                                              \
  X(SCAS)                                                                                                              \
  X(LOOP)                                                                                                              \
  X(LOOPE)                                                                                                             \
  X(LOOPNE)                                                                                                            \
  X(ENTER)                                                                                                             \
  X(LEAVE)                                                                                                             \
  X(ROL)                                                                                                               \
  X(ROR)                                                                                                               \
  X(RCL)                                                                                                               \
  X(RCR)                                                                                                               \
  X(SHL)                                                                                                               \
  X(SHR)                                                                                                               \
  X(SAR)                                                                                                               \
  X(NEG)                                                                                                               \
  X(NOT)                                                                                                               \
  X(MUL)                                                                                                               \
  X(DIV)                                                                                                               \
  X(IDIV)                                                                                                              \
  X(SEG_ES)                                                                                                            \
  X(SEG_CS)                                                                                                            \
  X(SEG_SS)                                                                                                            \
  X(SEG_DS)                                                                                                            \
  X(SEG_FS)                                                                                                            \
  X(SEG_GS)                                                                                                            \
  X(SAHF)                                                                                                              \
  X(LAHF)                                                                                                              \
  X(OPERAND_SIZE_PREFIX)                                                                                               \
  X(ADDRESS_SIZE_PREFIX)                                                                                               \
  X(LOCK)                                                                                                              \
  X(REP)                                                                                                               \
  X(REPNE)                                                                                                             \
  X(VEX1)                                                                                                              \
  X(VEX2)                                                                                                              \
  X(X87)                                                                                                               \
  X(SBB)                                                                                                               \
  X(DAA)                                                                                                               \
  X(DAS)                                                                                                               \
  X(AAA)                                                                                                               \
  X(AAS)                                                                                                               \
  X(GROUP1)                                                                                                            \
  X(GROUP1A)                                                                                                           \
  X(GROUP2)                                                                                                            \
  X(GROUP3)                                                                                                            \
  X(GROUP4)                                                                                                            \
  X(GROUP5)                                                                                                            \
  X(GROUP11)

enum class Mnemonic : std::uint16_t
{
#define X(name) name,
  MNEMONIC_LIST(X)
#undef X
};

constexpr std::string_view ToString(Mnemonic Value) noexcept
{
  switch(Value)
  {
#define X(name)                                                                                                        \
  case Mnemonic::name:                                                                                                 \
    return #name;

    MNEMONIC_LIST(X)

#undef X
  }

  return "INVALID";
}
