#pragma once

#include <cstddef>

enum class OperandSize : std::uint8_t
{
  // real mode
  Bits8,
  Bits16,
  // protected mode
  Bits32,
  // amd64 mode
  Bits64
};