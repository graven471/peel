#pragma once

#include <cstddef>
#include <string_view>
#include <print>
#include <utility>
#include "Opcodes.hpp"

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
  X(R11)                                                                                                               \
  X(R12)                                                                                                               \
  X(R13)                                                                                                               \
  X(R14)                                                                                                               \
  X(R15)                                                                                                               \
  X(CS)                                                                                                                \
  X(DS)                                                                                                                \
  X(ES)                                                                                                                \
  X(SS)                                                                                                                \
  X(FS)                                                                                                                \
  X(GS)                                                                                                                \
  X(CR0)                                                                                                               \
  X(CR1)                                                                                                               \
  X(CR2)                                                                                                               \
  X(CR3)                                                                                                               \
  X(CR4)                                                                                                               \
  X(CR8)                                                                                                               \
  X(RIP)                                                                                                               \
  X(EIP)                                                                                                               \
  X(RFLAGS)                                                                                                            \
  X(EFLAGS)

enum class Register : std::uint16_t
{
#define X(name) name,
  REGISTER_LIST(X)
#undef X
};

constexpr std::string_view ToString(Register Value) noexcept
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

#define GPR_REGISTERS(X)                                                                                               \
  X(0, RAX, EAX)                                                                                                       \
  X(1, RCX, ECX)                                                                                                       \
  X(2, RDX, EDX)                                                                                                       \
  X(3, RBX, EBX)                                                                                                       \
  X(4, RSP, ESP)                                                                                                       \
  X(5, RBP, EBP)                                                                                                       \
  X(6, RSI, ESI)                                                                                                       \
  X(7, RDI, EDI)

Register ResolveRegister(std::uint8_t Encoding, OperandSize Size)
{
  switch(Encoding)
  {
#define X(value, reg64, reg32)                                                                                         \
  case value:                                                                                                          \
    return Size == OperandSize::Bits64 ? Register::reg64 : Register::reg32;

    GPR_REGISTERS(X)

#undef X

    default:
      std::println(stderr, "Invalid operand size");
      std::unreachable();
  }
}