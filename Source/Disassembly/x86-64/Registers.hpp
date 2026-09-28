#pragma once

#include <cstddef>
#include <string_view>
#include <print>
#include <utility>
#include "Types.hpp"

#define REGISTER_LIST(X)                                                                                               \
  X(AL)                                                                                                                \
  X(CL)                                                                                                                \
  X(DL)                                                                                                                \
  X(BL) X(AH) X(CH) X(DH) X(BH) X(AX) X(CX) X(DX) X(BX) X(SP) X(BP) X(SI) X(DI) X(EAX) X(ECX) X(EDX) X(EBX) X(ESP)     \
      X(EBP) X(ESI) X(EDI) X(RAX) X(RCX) X(RDX) X(RBX) X(RSP) X(RBP) X(RSI) X(RDI) X(R8) X(R9) X(R10) X(R11) X(R12)    \
          X(R13) X(R14) X(R15) X(R8D) X(R9D) X(R10D) X(R11D) X(R12D) X(R13D) X(R14D) X(R15D) X(R8W) X(R9W) X(R10W)     \
              X(R11W) X(R12W) X(R13W) X(R14W) X(R15W) X(CS) X(DS) X(ES) X(SS) X(FS) X(GS) X(CR0) X(CR1) X(CR2) X(CR3)  \
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

#define GPR_REGISTERS(X)                                                                                               \
  X(0, RAX, EAX, AX)                                                                                                   \
  X(1, RCX, ECX, CX)                                                                                                   \
  X(2, RDX, EDX, DX)                                                                                                   \
  X(3, RBX, EBX, BX)                                                                                                   \
  X(4, RSP, ESP, SP)                                                                                                   \
  X(5, RBP, EBP, BP)                                                                                                   \
  X(6, RSI, ESI, SI)                                                                                                   \
  X(7, RDI, EDI, DI)                                                                                                   \
  X(8, R8, R8D, R8W)                                                                                                   \
  X(9, R9, R9D, R9W)                                                                                                   \
  X(10, R10, R10D, R10W)                                                                                               \
  X(11, R11, R11D, R11W)                                                                                               \
  X(12, R12, R12D, R12W)                                                                                               \
  X(13, R13, R13D, R13W)                                                                                               \
  X(14, R14, R14D, R14W)                                                                                               \
  X(15, R15, R15D, R15W)

constexpr Register ResolveRegister(std::uint8_t Encoding, OperandSize Size)
{
  switch(Encoding)
  {
#define X(value, reg64, reg32, reg16)                                                                                  \
  case value:                                                                                                          \
    if(Size == OperandSize::Bits64)                                                                                    \
      return Register::reg64;                                                                                          \
    if(Size == OperandSize::Bits32)                                                                                    \
      return Register::reg32;                                                                                          \
    return Register::reg16;

    GPR_REGISTERS(X)
#undef X

    default:
      std::unreachable();
  }
}