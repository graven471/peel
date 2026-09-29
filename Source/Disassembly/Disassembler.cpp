#include "Disassembler.hpp"

#include <algorithm>
#include <cassert>
#include <print>
#include <ranges>
#include <span>

#include "x86-64/Opcodes.hpp"
#include "x86-64/Prefixes.hpp"
#include "x86-64/Instructions.hpp"
#include "x86-64/Registers.hpp"
#include "x86-64/Types.hpp"

void Disassembler::do_it()
{
  constexpr std::array text_section_name{'.', 't', 'e', 'x', 't', '\0', '\0', '\0'};

  auto it = std::ranges::find_if(image->SectionHeaders, [&](const ImageSection& section) {
    return std::ranges::equal(section.Header.Name, text_section_name);
  });

  if(it == image->SectionHeaders.end()) [[unlikely]]
  {
    std::println(stderr, "no .text section is present");
    return;
  }

  ImageSection& text_section = *it;

  std::span<const std::byte> instruction_encoding = text_section.Data;

  //std::span<const std::byte> instruction_encoding = test_bytes_span;

  while(!instruction_encoding.empty())
  {
    prefix_scanner(instruction_encoding);
    OpcodeResult opcode = opcode_scanner(instruction_encoding);

    InstructionDesc opcode_metadata = OPCODE_TABLE[std::to_integer<std::uint8_t>(opcode.bytes[0])];

    if(opcode_metadata.mod_rm)
    {
      const std::byte mod_rm_byte = instruction_encoding[0];

      ModRM mod_rm         = mod_rm_scanner(mod_rm_byte);
      instruction_encoding = instruction_encoding.subspan(1);

      build_mod_rm_instruction(mod_rm, opcode_metadata, instruction_encoding);
    }
    else
    {
      build_immediate_instruction(opcode_metadata, instruction_encoding);
    }
  }
}

void Disassembler::prefix_scanner(std::span<const std::byte>& bytes)
{
  auto prefix_bytes = bytes.first(std::min<std::size_t>(5, bytes.size()));

  std::uint8_t                  prefix_count{0};
  std::array<PrefixMetadata, 5> prefixes{};

  for(std::uint8_t index = 0; index < prefix_bytes.size(); ++index)
  {
    auto meta = PREFIX_TABLE[std::to_integer<std::uint8_t>(prefix_bytes[index])];

    if(meta.is_prefix)
    {
      prefixes[index] = PrefixMetadata{.type = meta.type, .is_prefix = true};

      prefix_count++;
    }
    else
    {
      break;
    }
  }

  assert(prefix_count <= 5);

  bytes = bytes.subspan(prefix_count);
}

OpcodeResult Disassembler::opcode_scanner(std::span<const std::byte>& bytes)
{
  assert(!bytes.empty());

  if(bytes[0] != OPCODE_ESCAPE)
  {
    const std::byte opcode = bytes[0];
    bytes                  = bytes.subspan(1);

    return OpcodeResult{.bytes = {opcode, std::byte{0}, std::byte{0}}, .length = 1};
  }

  if(bytes[1] == OPCODE_MAP2_SELECT || bytes[1] == OPCODE_MAP3_SELECT)
  {
    assert(bytes.size() >= 3 &&
           "opcode contains OPCODE_MAP byte so Bytes must need to be atleast "
           "3-bytes");

    const std::byte map    = bytes[1];
    const std::byte opcode = bytes[2];

    bytes = bytes.subspan(3);

    return OpcodeResult{.bytes = map == OPCODE_MAP2_SELECT ? build_opcode_map2(opcode) : build_opcode_map3(opcode), .length = 3};
  }

  assert(bytes.size() >= 2 &&
         "opcode contains OPCODE_ESCAPE bit so Bytes must need to be atleast "
         "2-bytes");

  std::array<std::byte, 2> opcodes = build_two_bytes_opcode(bytes[1]);

  bytes = bytes.subspan(2);

  return OpcodeResult{.bytes = {opcodes[0], opcodes[1], std::byte{0x0}}, .length = 2};
}

ModRM Disassembler::mod_rm_scanner(const std::byte byte)
{
  /*
    example: 11 001 100
             |   |   |
            mod  reg  r/m
  */

  const std::uint8_t value = std::to_integer<std::uint8_t>(byte);

  const std::uint8_t mod = (value & 0xC0) >> 6;
  const std::uint8_t reg = (value & 0x38) >> 3;
  const std::uint8_t rm  = value & 0x07;

  return ModRM{.mod = mod, .reg = reg, .rm = rm};
}

ModRMOperandInfo Disassembler::resolve_mod_rm(const ModRM& mod_rm, const InstructionDesc& metadata)
{
  struct ModRMOperandInfo info{};

  // todo: replace operand size with REX.W prefix check
  info.rm = resolve_gp_register(mod_rm.rm, OperandSize::Bits64);

  info.reg = resolve_gp_register(mod_rm.reg, OperandSize::Bits64);

  if(metadata.destination == OperandCode::Ev && metadata.source == OperandCode::Gv)
  {
    info.destination = ModRMField::Rm;
    info.source      = ModRMField::Reg;
  }
  else if(metadata.destination == OperandCode::Gv && metadata.source == OperandCode::Ev)
  {
    info.destination = ModRMField::Reg;
    info.source      = ModRMField::Rm;
  }
  else
  {
    // generic fallback for now
    info.destination = ModRMField::Reg;
    info.source      = ModRMField::Rm;
  }

  switch(static_cast<ModRMMode>(mod_rm.mod))
  {
    case ModRMMode::Register:
      info.mode = ModRMMode::Register;
      break;

    case ModRMMode::MemoryNoDisplacement:
      info.mode = ModRMMode::MemoryNoDisplacement;
      break;

    case ModRMMode::MemoryDisp8:
      info.mode = ModRMMode::MemoryDisp8;
      break;

    case ModRMMode::MemoryDisp32:
      info.mode = ModRMMode::MemoryDisp32;
      break;

    default:
      std::println(stderr, "Invalid ModRM.mod");
      std::unreachable();
  }

  return info;
}

SIB Disassembler::get_sib_from_byte(std::uint8_t byte, std::uint8_t mod)
{
  return SIB{
      .scale = static_cast<std::int8_t>((byte >> 6) & 0x03),
      .index = static_cast<std::int8_t>(((byte >> 3) & 0x07) == 0b100 ? -1 : (byte >> 3) & 0x07),
      .base  = static_cast<std::int8_t>(((byte & 0x07) == 0b101 && mod == 0) ? -1 : byte & 0x07),
  };
}

void Disassembler::build_mod_rm_instruction(const ModRM& mod_rm, const InstructionDesc& metadata, std::span<const std::byte>& bytes)
{
  ModRMOperandInfo info = resolve_mod_rm(mod_rm, metadata);

  std::string_view mnemonic = to_string(metadata.mnemonic);

  auto build_base_address = [&]() -> EffectiveAddress {
    EffectiveAddress effective_address{};

    // sib exists iff mod != register and rm = 100
    if(info.mode != ModRMMode::Register && mod_rm.rm == 4)
    {
      std::uint8_t sib_byte{};

      std::memcpy(&sib_byte, bytes.data(), sizeof(sib_byte));

      bytes = bytes.subspan(sizeof(sib_byte));

      effective_address.sib = get_sib_from_byte(sib_byte, mod_rm.mod);
    }

    if(info.mode == ModRMMode::MemoryDisp8)
    {
      std::int8_t disp{};

      std::memcpy(&disp, bytes.data(), sizeof(std::int8_t));

      bytes = bytes.subspan(sizeof(std::int8_t));

      effective_address.displacement = disp;
    }
    else if(info.mode == ModRMMode::MemoryDisp32)
    {
      std::memcpy(&effective_address.displacement, bytes.data(), sizeof(std::int32_t));

      bytes = bytes.subspan(sizeof(std::int32_t));
    }

    return effective_address;
  };

  std::string_view source_register = info.source == ModRMField::Rm ? to_string(info.rm) : to_string(info.reg);

  std::string_view destination_register = info.destination == ModRMField::Rm ? to_string(info.rm) : to_string(info.reg);

  EffectiveAddress effective_address = build_base_address();

  //std::println("{} {}, {}", mnemonic, destination_register,
  //             info.mode == ModRMMode::Register ? std::string{source_register} :
  //                                                // FIXME: dangling lifetime issue fix it
  //                 effective_address.render_sib(OperandSize::Bits64, false, image->NTHeaders, mod_rm));
}

void Disassembler::build_immediate_instruction(const InstructionDesc& metadata, std::span<const std::byte>& bytes)
{
  if(metadata.destination == OperandCode::Iv || metadata.destination == OperandCode::Iz || metadata.source == OperandCode::Ib
     || metadata.source == OperandCode::Iv || metadata.source == OperandCode::Iz || metadata.source == OperandCode::Ib) [[likely]]
  {
    //std::println("imm: {:x}", std::to_integer<uint8_t>(bytes[0]));
  }
}