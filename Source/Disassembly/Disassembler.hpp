#pragma once

#include "Parser/PETypes.hpp"

struct OpcodeResult
{
  std::array<std::byte, 3> Bytes{};
  std::uint8_t             Length{};
};

struct ModRM
{
  std::uint8_t Mod{};
  std::uint8_t Reg{};
  std::uint8_t Rm{};
};

class Disassembler
{
public:
  explicit Disassembler(PEImage* InImage)
      : Image(InImage) {};

  void DoIt();

private:
  void         PrefixScanner(std::span<const std::byte>& Bytes);
  OpcodeResult OpcodeScanner(std::span<const std::byte>& Bytes);
  ModRM        ModRMScanner(const std::byte Byte);

  PEImage* Image = nullptr;
};
