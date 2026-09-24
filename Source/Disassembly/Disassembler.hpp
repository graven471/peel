#pragma once

#include "Parser/PETypes.hpp"
#include "x86-64/Registers.hpp"

struct InstructionDesc;

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

enum class ModRMField : std::uint8_t
{
  Reg,
  Rm
};

enum class ModRMMode : std::uint8_t
{
  MemoryNoDisplacement = 0,
  MemoryDisp8          = 1,
  MemoryDisp32         = 2,
  Register             = 3,
};

struct ModRMOperandInfo
{
  ModRMField Destination;
  ModRMField Source;

  ModRMMode Mode;

  Register Reg;
  Register Rm;
};

class Disassembler
{
public:
  explicit Disassembler(PEImage* InImage)
      : Image(InImage) {};

  void DoIt();

private:
  void             PrefixScanner(std::span<const std::byte>& Bytes);
  OpcodeResult     OpcodeScanner(std::span<const std::byte>& Bytes);
  ModRM            ModRMScanner(const std::byte Byte);
  ModRMOperandInfo ResolveModRM(const ModRM& ModRm, const InstructionDesc& MetaData);
  void BuildModRMInstruction(const ModRM& ModRm, const InstructionDesc& MetaData, std::span<const std::byte>& Bytes);

  PEImage* Image = nullptr;
};
