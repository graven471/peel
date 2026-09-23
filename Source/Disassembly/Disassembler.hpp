#pragma once

#include "Parser/PETypes.hpp"

struct OpcodeResult
{
  std::array<std::byte, 3> Bytes{};
  std::uint8_t             Length{};
};

class Disassembler
{
public:
  explicit Disassembler(PEImage* InImage)
      : Image(InImage) {};

  void DoIt();

private:
  void         PrefixScanner(std::span<const std::byte> Bytes);
  OpcodeResult OpcodeScanner(std::span<const std::byte> Bytes);

  PEImage* Image = nullptr;
};
