#pragma once

#include <cstddef>
#include <string_view>
#include <print>
#include <utility>
#include <cassert>
#include "Types.hpp"

#define REGISTER_LIST(X)                                                                                               \
  X(AL)                                                                                                                \
  X(CL)                                                                                                                \
  X(DL)                                                                                                                \
  X(BL)                                                                                                                \
  X(AH)                                                                                                                \
  X(CH)                                                                                                                \
  X(DH)                                                                                                                \
  X(BH)                                                                                                                \
  X(AX)                                                                                                                \
  X(CX)                                                                                                                \
  X(DX)                                                                                                                \
  X(BX)                                                                                                                \
  X(SP)                                                                                                                \
  X(BP)                                                                                                                \
  X(SI)                                                                                                                \
  X(DI)                                                                                                                \
  X(EAX)                                                                                                               \
  X(ECX)                                                                                                               \
  X(EDX)                                                                                                               \
  X(EBX)                                                                                                               \
  X(ESP)                                                                                                               \
  X(EBP)                                                                                                               \
  X(ESI)                                                                                                               \
  X(EDI)                                                                                                               \
  X(RAX)                                                                                                               \
  X(RCX)                                                                                                               \
  X(RDX)                                                                                                               \
  X(RBX)                                                                                                               \
  X(RSP)                                                                                                               \
  X(RBP)                                                                                                               \
  X(RSI)                                                                                                               \
  X(RDI)                                                                                                               \
  X(R8)                                                                                                                \
  X(R9)                                                                                                                \
  X(R10)                                                                                                               \
  X(R11) X(R12) X(R13) X(R14) X(R15) X(R8D) X(R9D) X(R10D) X(R11D) X(R12D) X(R13D) X(R14D) X(R15D) X(R8W) X(R9W)       \
      X(R10W) X(R11W) X(R12W) X(R13W) X(R14W) X(R15W) X(CS) X(DS) X(ES) X(SS) X(FS) X(GS) X(CR0) X(CR1) X(CR2) X(CR3)  \
          X(CR4) X(CR8) X(RIP) X(EIP) X(RFLAGS) X(EFLAGS)

enum class Register : std::uint16_t
{
#define X(name) name,
  REGISTER_LIST(X)
#undef X
};

constexpr std::string ToString(Register Value) noexcept
{
  switch(Value)
  {
#define X(name)                                                                                                        \
  case Register::name:                                                                                                 \
    return #name;

    REGISTER_LIST(X)

#undef X
  }

  return "unknown";
}

struct GrpRegisters
{
  Register reg64;
  Register reg32;
  Register reg16;
};

static constexpr GrpRegisters GRP_REGISTER_TABLE[16] = {
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

static constexpr Register ResolveRegister(std::uint8_t encoding, OperandSize opsize) noexcept
{
  assert(encoding < 16 && "there exists only 16 general purpose registers");

  GrpRegisters gp_register = GRP_REGISTER_TABLE[encoding];

  if(opsize == OperandSize::Bits64)
    return gp_register.reg64;

  if(opsize == OperandSize::Bits32)
    return gp_register.reg32;

  return gp_register.reg16;
}