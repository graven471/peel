#pragma once

#include "Parser/PETypes.hpp"
#include "x86-64/Registers.hpp"

#include <cassert>
#include <format>

struct InstructionDesc;

struct OpcodeResult
{
  std::array<std::byte, 3> bytes{};
  std::uint8_t             length{};
};

struct ModRM
{
  std::uint8_t mod{};
  std::uint8_t reg{};
  std::uint8_t rm{};
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
  ModRMField destination;
  ModRMField source;

  ModRMMode mode;

  Register reg;
  Register rm;
};

struct SIB
{
  // The scale field is used to specify the scale factor used in computing the
  // scale * index portion of the effective address. In normal usage scale
  // represents the size of data elements in an array expressed in number of bytes.
  std::int8_t scale{-1};

  // The index field is used to specify the register containing the index portion
  // of the indexed register-indirect effective address.
  std::int8_t index{-1};

  // The base field is used to specify the register containing the base address
  // portion of the indexed register-indirect effective address.
  std::int8_t base{};

  constexpr std::uint8_t decode_scale() const noexcept
  {
    constexpr std::uint8_t table[] = {1, 2, 4, 8};
    return table[scale & 0b11];
  }
};

// when both SIB and disp{8,32} is present in instruction
// there exists a instruction where sib can be optional or displacement or both
struct EffectiveAddress
{
  SIB          sib{};
  std::int32_t displacement{};

  bool is_sib() const noexcept { return sib.base != -1 && sib.index != -1 && sib.scale != 0; }

  // this does not belong here
  std::string render_sib(OperandSize size, bool address_override_prefix, const ImageNTHeaders& nt_headers, const ModRM& mod_rm) const noexcept
  {
    auto decode_effective_address = [&](OperandSize op_size) -> std::string {
      std::string expression = "[";

      if(!is_sib())
      {
        // this is special case
        // in long mode mode this maps to [RIP + disp32]
        // in compatibility mode it maps to disp32
        if(mod_rm.mod == 0 && mod_rm.rm == 5)
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
          expression += to_string(resolve_gp_register(mod_rm.rm, op_size));

          if(displacement != 0)
          {
            displacement < 0 ? expression += std::format(" - 0x{:x}", std::abs(displacement)) :
                               expression += std::format(" + 0x{:x}", displacement);
          }
        }

        expression += "]";
        return expression;
      }

      if(sib.base != -1)
      {
        expression += to_string(resolve_gp_register(static_cast<std::uint8_t>(sib.base), op_size));
        expression += " + ";
      }

      if(sib.index != -1)
      {
        expression += to_string(resolve_gp_register(static_cast<std::uint8_t>(sib.index), op_size));
        expression += "*";
      }

      sib.scale == 0 ? expression += "1" : expression += std::to_string(sib.decode_scale());

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
      // in this case the operating mode is 64-bit but 67H is address override prefix
      // according to manual if we are in 64-bit mode and if 67H is in instruction prefix
      // then we should use 32-bit as address-size

      //[base + index*scale + disp]
      return decode_effective_address(address_override_prefix ? OperandSize::Bits32 : OperandSize::Bits64);
    }

    // generic case like protected mode (compatibility mode) etc
    // the check is if address_override_prefix then 16-bit address else 32-bit
    //if(nt_headers.FileHeader.Machine == MachineType::MACHINE_I386 && nt_headers.Format == PEFormat::PE32) [[unlikely]]
    //{
    //
    //}

    return decode_effective_address(address_override_prefix ? OperandSize::Bits16 : OperandSize::Bits32);
  }
};

class Disassembler
{
public:
  explicit Disassembler(PEImage* image)
      : image(image) {};

  void disassemble() noexcept;

private:
  void             scan_prefixes(std::span<const std::byte>& bytes);
  OpcodeResult     scan_opcode(std::span<const std::byte>& bytes);
  ModRM            scan_mod_rm(const std::byte byte);
  ModRMOperandInfo resolve_mod_rm(const ModRM& mod_rm, const InstructionDesc& metadata);
  SIB              decode_sib(std::uint8_t byte, std::uint8_t mod);

  void build_mod_rm_instruction(const ModRM& mod_rm, const InstructionDesc& metadata, std::span<const std::byte>& bytes);
  void build_immediate_instruction(const InstructionDesc& metadata, std::span<const std::byte>& bytes);

  PEImage* image = nullptr;
};