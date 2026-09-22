#pragma once

#include "Parser/PETypes.hpp"

class Disassembler
{
public:
  explicit Disassembler(PEImage* InImage)
      : Image(InImage) {};

  void DoIt();

private:
  void PrefixScanner(std::span<const std::byte> Bytes);
  void OpCodeScanner(std::span<const std::byte> Bytes);

  PEImage* Image = nullptr;
};
