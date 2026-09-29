#pragma once

#include <cstddef>
#include <string_view>

// TODO: replace this with c++ 26 static reflection when msvc adds support for it

#define MNEMONIC_LIST(X)                                                                                               \
  X(INVALID, "invalid")                                                                                                \
  X(MOV, "mov")                                                                                                        \
  X(MOVS, "movs")                                                                                                      \
  X(MOVSXD, "movsxd")                                                                                                  \
  X(MOVBE, "movbe")                                                                                                    \
  X(ESCAPE_2BYTE, "escape_2byte")                                                                                      \
  X(LEA, "lea")                                                                                                        \
  X(RET, "ret")                                                                                                        \
  X(RETF, "retf")                                                                                                      \
  X(ADD, "add")                                                                                                        \
  X(SUB, "sub")                                                                                                        \
  X(IMUL, "imul")                                                                                                      \
  X(CBW, "cbw")                                                                                                        \
  X(CWD, "cwd")                                                                                                        \
  X(INS, "ins")                                                                                                        \
  X(ADC, "adc")                                                                                                        \
  X(OR, "or")                                                                                                          \
  X(AND, "and")                                                                                                        \
  X(XOR, "xor")                                                                                                        \
  X(INC, "inc")                                                                                                        \
  X(DEC, "dec")                                                                                                        \
  X(CMP, "cmp")                                                                                                        \
  X(PUSH, "push")                                                                                                      \
  X(PUSHF, "pushf")                                                                                                    \
  X(PUSHAD, "pushad")                                                                                                  \
  X(POP, "pop")                                                                                                        \
  X(POPF, "popf")                                                                                                      \
  X(POPAD, "popad")                                                                                                    \
  X(TEST, "test")                                                                                                      \
  X(INT, "int")                                                                                                        \
  X(INT1, "int1")                                                                                                      \
  X(INT3, "int3")                                                                                                      \
  X(INTO, "into")                                                                                                      \
  X(IRET, "iret")                                                                                                      \
  X(IN, "in")                                                                                                          \
  X(OUT, "out")                                                                                                        \
  X(OUTS, "outs")                                                                                                      \
  X(JO, "jo")                                                                                                          \
  X(JNO, "jno")                                                                                                        \
  X(JB, "jb")                                                                                                          \
  X(JNB, "jnb")                                                                                                        \
  X(JZ, "jz")                                                                                                          \
  X(JNZ, "jnz")                                                                                                        \
  X(JBE, "jbe")                                                                                                        \
  X(JA, "ja")                                                                                                          \
  X(JS, "js")                                                                                                          \
  X(JNS, "jns")                                                                                                        \
  X(JP, "jp")                                                                                                          \
  X(JNP, "jnp")                                                                                                        \
  X(JL, "jl")                                                                                                          \
  X(JGE, "jge")                                                                                                        \
  X(JLE, "jle")                                                                                                        \
  X(JG, "jg")                                                                                                          \
  X(JMP, "jmp")                                                                                                        \
  X(JRCXZ, "jrcxz")                                                                                                    \
  X(CALL, "call")                                                                                                      \
  X(WAIT, "wait")                                                                                                      \
  X(BOUND, "bound")                                                                                                    \
  X(XCHG, "xchg")                                                                                                      \
  X(XLAT, "xlat")                                                                                                      \
  X(NOP, "nop")                                                                                                        \
  X(HLT, "hlt")                                                                                                        \
  X(CMC, "cmc")                                                                                                        \
  X(CLC, "clc")                                                                                                        \
  X(STC, "stc")                                                                                                        \
  X(CLI, "cli")                                                                                                        \
  X(STI, "sti")                                                                                                        \
  X(CLD, "cld")                                                                                                        \
  X(STD, "std")                                                                                                        \
  X(CMPS, "cmps")                                                                                                      \
  X(LODS, "lods")                                                                                                      \
  X(STOS, "stos")                                                                                                      \
  X(SCAS, "scas")                                                                                                      \
  X(LOOP, "loop")                                                                                                      \
  X(LOOPE, "loope")                                                                                                    \
  X(LOOPNE, "loopne")                                                                                                  \
  X(ENTER, "enter")                                                                                                    \
  X(LEAVE, "leave")                                                                                                    \
  X(ROL, "rol")                                                                                                        \
  X(ROR, "ror")                                                                                                        \
  X(RCL, "rcl")                                                                                                        \
  X(RCR, "rcr")                                                                                                        \
  X(SHL, "shl")                                                                                                        \
  X(SHR, "shr")                                                                                                        \
  X(SAR, "sar")                                                                                                        \
  X(NEG, "neg")                                                                                                        \
  X(NOT, "not")                                                                                                        \
  X(MUL, "mul")                                                                                                        \
  X(DIV, "div")                                                                                                        \
  X(IDIV, "idiv")                                                                                                      \
  X(SEG_ES, "es")                                                                                                      \
  X(SEG_CS, "cs")                                                                                                      \
  X(SEG_SS, "ss")                                                                                                      \
  X(SEG_DS, "ds")                                                                                                      \
  X(SEG_FS, "fs")                                                                                                      \
  X(SEG_GS, "gs")                                                                                                      \
  X(SAHF, "sahf")                                                                                                      \
  X(LAHF, "lahf")                                                                                                      \
  X(OPERAND_SIZE_PREFIX, "operand_size_prefix")                                                                        \
  X(ADDRESS_SIZE_PREFIX, "address_size_prefix")                                                                        \
  X(LOCK, "lock")                                                                                                      \
  X(REP, "rep")                                                                                                        \
  X(REPNE, "repne")                                                                                                    \
  X(VEX1, "vex1")                                                                                                      \
  X(VEX2, "vex2")                                                                                                      \
  X(X87, "x87")                                                                                                        \
  X(SBB, "sbb")                                                                                                        \
  X(DAA, "daa")                                                                                                        \
  X(DAS, "das")                                                                                                        \
  X(AAA, "aaa")                                                                                                        \
  X(AAS, "aas")                                                                                                        \
  X(GROUP1, "group1")                                                                                                  \
  X(GROUP1A, "group1a")                                                                                                \
  X(GROUP2, "group2")                                                                                                  \
  X(GROUP3, "group3")                                                                                                  \
  X(GROUP4, "group4")                                                                                                  \
  X(GROUP5, "group5")                                                                                                  \
  X(GROUP11, "group11")

enum class Mnemonic : std::uint16_t
{
#define X(name) name,
  MNEMONIC_LIST(X)
#undef X
      count
};

static constexpr std::array<std::string_view, static_cast<std::size_t>(Mnemonic::count)> MNEMONIC_STRINGS{
#define X(name, string) string,
    MNEMONIC_LIST(X)
#undef X
};

constexpr std::string_view to_string(Mnemonic mnemonic) noexcept
{
  assert(static_cast<std::size_t>(mnemonic) < static_cast<std::size_t>(Mnemonic::count) && "invalid mnemonic access");
  return MNEMONIC_STRINGS[static_cast<std::size_t>(mnemonic)];
}
