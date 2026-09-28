#pragma once

#include "Parser/PETypes.hpp"
#include "x86-64/Registers.hpp"
#include <format>
#include <cassert>

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

struct SIB
{
  //The scale field is used to specify the scale factor used in computing the
  //scale* index portion of the effective
  //address.In normal usage scale represents the size of data elements in an array expressed in number of bytes.
  std::int8_t Scale{-1};
  //The index field is used to specify the register containing the index portion of
  //the indexed register - indirect effective address
  std::int8_t Index{-1};
  //The base field is used to specify the register containing the base address
  //portion of the indexed register - indirect effective address
  std::int8_t Base{};

  constexpr std::uint8_t decode_scale() const noexcept
  {
    constexpr std::uint8_t table[] = {1, 2, 4, 8};
    return table[Scale & 0b11];
  }
};

// when both SIB and disp{8,32} is present in instruction
// there exists a instruction where sib can be optional or displacement or both
struct EffectiveAddress
{
  SIB          sib{};
  std::int32_t displacement{};

  bool is_sib() const noexcept { return sib.Base != -1 && sib.Index != -1 && sib.Scale != 0; }

  // this does not belong here
  std::string render_sib(OperandSize size, bool address_override_prefix, const ImageNTHeaders& nt_headers, const ModRM& modrm) const noexcept
  {
    // 67H(address_override_prefix) prefix can overwrite this
    // manual has defined like this:
    // long-mode mode default size 64-bit, ea = 64-bit, 67H required no
    // compatibility mode default size 32-bit, ea = 32-bit with 67H its 16-bit
    // protected-mode or real mode default size 32-bit or 16-bit ea = 32-bit or 16-bit with 67h

    auto build_instruction = [&](OperandSize op_size) -> std::string {
      std::string expression = "[";

      if(!is_sib())
      {
        // this is special case
        // in long mode mode this maps to [RIP + disp32]
        // in compatibility mode it maps to disp32
        if(modrm.Mod == 0 && modrm.Rm == 5)
        {
          // RIP-relative or disp32
          if(nt_headers.FileHeader.Machine == MachineType::MACHINE_AMD64)
            expression += std::format("RIP + 0x{:x}", displacement);
          else
            expression += std::format("0x{:x}", displacement);
        }
        else
        {
          // regular [reg] or [reg + disp]
          expression += ToString(ResolveRegister(modrm.Rm, op_size));
          if(displacement != 0)
            displacement < 0 ? expression += std::format(" - 0x{:x}", std::abs(displacement)) :
                               expression += std::format(" + 0x{:x}", displacement);
        }

        expression += "]";
        return expression;
      }

      if(sib.Base != -1)
      {
        expression += ToString(ResolveRegister(static_cast<std::uint8_t>(sib.Base), op_size));
        expression += " + ";
      }

      if(sib.Index != -1)
      {
        expression += ToString(ResolveRegister(static_cast<std::uint8_t>(sib.Index), op_size));
        expression += "*";
      }

      sib.Scale == 0 ? expression += "1" : expression += std::to_string(sib.decode_scale());

      if(displacement != 0)
      {
        displacement < 0 ? expression += std::format(" - 0x{:x}", std::abs(displacement)) :
                           expression += std::format(" + 0x{:x}", displacement);
      }

      expression += "]";

      return expression;
    };

    if(nt_headers.FileHeader.Machine == MachineType::MACHINE_AMD64 && nt_headers.Format == PEFormat::PE64) [[likely]]
    {
      // in this case the operating mode is 64-bit but 67H prefix is address override prefix
      // according to manual if we are in 64-bit mode and if 67H is in instruction prefix
      // then we should use 32-bit as address-size

      //[base + index*scale + disp]
      return build_instruction(address_override_prefix ? OperandSize::Bits32 : OperandSize::Bits64);
    }

    // generic case like protected mode (compatibility mode) etc
    // the check is if address_override_prefix then 16-bit address else 32-bit
    //if(nt_headers.FileHeader.Machine == MachineType::MACHINE_I386 && nt_headers.Format == PEFormat::PE32) [[unlikely]]
    //{
    //
    //}

    return build_instruction(address_override_prefix ? OperandSize::Bits16 : OperandSize::Bits32);
  };
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
  SIB              GetSibFromByte(std::uint8_t Byte, std::uint8_t mod);
  void BuildModRMInstruction(const ModRM& ModRm, const InstructionDesc& MetaData, std::span<const std::byte>& Bytes);

  PEImage* Image = nullptr;
};
