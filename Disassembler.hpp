#pragma once

#include "PETypes.hpp"

class Disassembler {
 public:
  explicit Disassembler(PEImage* InImage) : Image(InImage) {};

  void DoIt();

 private:
  PEImage* Image = nullptr;
};
