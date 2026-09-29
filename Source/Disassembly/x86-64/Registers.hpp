#pragma once

#include <cstddef>
#include <string_view>
#include <print>
#include <utility>
#include <cassert>
#include "Types.hpp"

#define REGISTER_LIST(X)                                                                                               \
  X(AL, "al")                                                                                                          \
  X(CL, "cl")                                                                                                          \
  X(DL, "dl")                                                                                                          \
  X(BL, "bl")                                                                                                          \
  X(AH, "ah")                                                                                                          \
  X(CH, "ch")                                                                                                          \
  X(DH, "dh")                                                                                                          \
  X(BH, "bh")                                                                                                          \
  X(AX, "ax")                                                                                                          \
  X(CX, "cx")                                                                                                          \
  X(DX, "dx")                                                                                                          \
  X(BX, "bx")                                                                                                          \
  X(SP, "sp")                                                                                                          \
  X(BP, "bp")                                                                                                          \
  X(SI, "si")                                                                                                          \
  X(DI, "di")                                                                                                          \
  X(EAX, "eax")                                                                                                        \
  X(ECX, "ecx")                                                                                                        \
  X(EDX, "edx")                                                                                                        \
  X(EBX, "ebx")                                                                                                        \
  X(ESP, "esp")                                                                                                        \
  X(EBP, "ebp")                                                                                                        \
  X(ESI, "esi")                                                                                                        \
  X(EDI, "edi")                                                                                                        \
  X(RAX, "rax")                                                                                                        \
  X(RCX, "rcx")                                                                                                        \
  X(RDX, "rdx")                                                                                                        \
  X(RBX, "rbx")                                                                                                        \
  X(RSP, "rsp")                                                                                                        \
  X(RBP, "rbp")                                                                                                        \
  X(RSI, "rsi")                                                                                                        \
  X(RDI, "rdi")                                                                                                        \
  X(R8, "r8")                                                                                                          \
  X(R9, "r9")                                                                                                          \
  X(R10, "r10")                                                                                                        \
  X(R11, "r11")                                                                                                        \
  X(R12, "r12")                                                                                                        \
  X(R13, "r13")                                                                                                        \
  X(R14, "r14")                                                                                                        \
  X(R15, "r15")                                                                                                        \
  X(R8D, "r8d")                                                                                                        \
  X(R9D, "r9d")                                                                                                        \
  X(R10D, "r10d")                                                                                                      \
  X(R11D, "r11d")                                                                                                      \
  X(R12D, "r12d")                                                                                                      \
  X(R13D, "r13d")                                                                                                      \
  X(R14D, "r14d")                                                                                                      \
  X(R15D, "r15d")                                                                                                      \
  X(R8W, "r8w")                                                                                                        \
  X(R9W, "r9w")                                                                                                        \
  X(R10W, "r10w")                                                                                                      \
  X(R11W, "r11w")                                                                                                      \
  X(R12W, "r12w")                                                                                                      \
  X(R13W, "r13w")                                                                                                      \
  X(R14W, "r14w")                                                                                                      \
  X(R15W, "r15w")                                                                                                      \
  X(CS, "cs")                                                                                                          \
  X(DS, "ds")                                                                                                          \
  X(ES, "es")                                                                                                          \
  X(SS, "ss")                                                                                                          \
  X(FS, "fs")                                                                                                          \
  X(GS, "gs")                                                                                                          \
  X(CR0, "cr0")                                                                                                        \
  X(CR2, "cr2")                                                                                                        \
  X(CR3, "cr3")                                                                                                        \
  X(CR4, "cr4")                                                                                                        \
  X(CR8, "cr8")                                                                                                        \
  X(RIP, "rip")                                                                                                        \
  X(EIP, "eip")                                                                                                        \
  X(RFLAGS, "rflags")                                                                                                  \
  X(EFLAGS, "eflags")

enum class Register : std::uint8_t
{
#define X(name, string) name,
  REGISTER_LIST(X)
#undef X
      count
};

constexpr auto register_index(Register reg) noexcept
{
  return std::to_underlying(reg);
}

static constexpr std::array<std::string_view, std::to_underlying(Register::count)> register_strings{
#define X(name, string) string,
    REGISTER_LIST(X)
#undef X
};

constexpr std::string_view to_string(Register reg) noexcept
{
  assert(std::to_underlying(reg) < std::to_underlying(Register::count) && "out of bound register access");
  return register_strings[std::to_underlying(reg)];
}

struct GpRegisters
{
  Register reg64;
  Register reg32;
  Register reg16;
};

static constexpr GpRegisters grp_register_table[16] = {
    {.reg64 = Register::RAX, .reg32 = Register::EAX, .reg16 = Register::AX},
    {.reg64 = Register::RCX, .reg32 = Register::ECX, .reg16 = Register::CX},
    {.reg64 = Register::RDX, .reg32 = Register::EDX, .reg16 = Register::DX},
    {.reg64 = Register::RBX, .reg32 = Register::EBX, .reg16 = Register::BX},
    {.reg64 = Register::RSP, .reg32 = Register::ESP, .reg16 = Register::SP},
    {.reg64 = Register::RBP, .reg32 = Register::EBP, .reg16 = Register::BP},
    {.reg64 = Register::RSI, .reg32 = Register::ESI, .reg16 = Register::SI},
    {.reg64 = Register::RDI, .reg32 = Register::EDI, .reg16 = Register::DI},
    {.reg64 = Register::R8, .reg32 = Register::R8D, .reg16 = Register::R8W},
    {.reg64 = Register::R9, .reg32 = Register::R9D, .reg16 = Register::R9W},
    {.reg64 = Register::R10, .reg32 = Register::R10D, .reg16 = Register::R10W},
    {.reg64 = Register::R11, .reg32 = Register::R11D, .reg16 = Register::R11W},
    {.reg64 = Register::R12, .reg32 = Register::R12D, .reg16 = Register::R12W},
    {.reg64 = Register::R13, .reg32 = Register::R13D, .reg16 = Register::R13W},
    {.reg64 = Register::R14, .reg32 = Register::R14D, .reg16 = Register::R14W},
    {.reg64 = Register::R15, .reg32 = Register::R15D, .reg16 = Register::R15W},
};

static constexpr Register resolve_gp_register(std::uint8_t encoding, OperandSize operand_size) noexcept
{
  assert(encoding < 16 && "there exists only 16 general purpose registers");

  const GpRegisters& gp_registers = grp_register_table[encoding];

  if(operand_size == OperandSize::Bits64)
    return gp_registers.reg64;

  if(operand_size == OperandSize::Bits32)
    return gp_registers.reg32;

  return gp_registers.reg16;
}