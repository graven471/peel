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

void Disassembler::DoIt()
{
  constexpr std::array TextSectionName{'.', 't', 'e', 'x', 't', '\0', '\0', '\0'};

  auto It = std::ranges::find_if(Image->SectionHeaders, [&](const ImageSection& Section) {
    return std::ranges::equal(Section.Header.Name, TextSectionName);
  });

  if(It == Image->SectionHeaders.end()) [[unlikely]]
  {
    std::println(stderr, "no .text section is present");
    return;
  }

  ImageSection& TextSection = *It;

  std::span<const std::byte> InstructionEncoding = TextSection.Data;

  //std::span<const std::byte> InstructionEncoding = TestBytesSpan;

  while(!InstructionEncoding.empty())
  {
    PrefixScanner(InstructionEncoding);
    OpcodeResult Opcode = OpcodeScanner(InstructionEncoding);

    InstructionDesc OpcodeMetadata = OPCODE_TABLE[std::to_integer<std::uint8_t>(Opcode.Bytes[0])];

    if(OpcodeMetadata.ModRM)
    {
      const std::byte ModRMByte = InstructionEncoding[0];

      ModRM ModRM         = ModRMScanner(ModRMByte);
      InstructionEncoding = InstructionEncoding.subspan(1);

      BuildModRMInstruction(ModRM, OpcodeMetadata, InstructionEncoding);
    }
    else
    {
      build_immediate_instruction(OpcodeMetadata, InstructionEncoding);
    }
  }
}

void Disassembler::PrefixScanner(std::span<const std::byte>& Bytes)
{
  auto                          prefix_bytes = Bytes.first(std::min<std::size_t>(5, Bytes.size()));
  std::uint8_t                  prefix_count{0};
  std::array<PrefixMetadata, 5> prefixes{};


  for(std::uint8_t index = 0; index < prefix_bytes.size(); ++index)
  {
    auto meta = PREFIX_TABLE[std::to_integer<std::uint8_t>(prefix_bytes[index])];

    if(meta.is_prefix)
    {
      prefixes[index] = PrefixMetadata{.Type = meta.Type, .is_prefix = true};
      prefix_count++;
    }
    else
    {
      break;
    }
  }

  assert(prefix_count <= 5);

  Bytes = Bytes.subspan(prefix_count);
}

OpcodeResult Disassembler::OpcodeScanner(std::span<const std::byte>& Bytes)
{
  assert(!Bytes.empty());

  if(Bytes[0] != OPCODE_ESCAPE)
  {
    const std::byte Opcode = Bytes[0];

    // slide instruction
    Bytes = Bytes.subspan(1);
    return OpcodeResult{.Bytes = {Opcode, std::byte{0}, std::byte{0}}, .Length = 1};
  }

  if(Bytes[1] == OPCODE_MAP2_SELECT || Bytes[1] == OPCODE_MAP3_SELECT)
  {
    assert(Bytes.size() >= 3 &&
           "opcode contains OPCODE_MAP byte so Bytes must need to be atleast "
           "3-bytes");

    // get the view to opcodes and slide bytes so it can point to after opcode
    const std::byte Map    = Bytes[1];
    const std::byte Opcode = Bytes[2];
    Bytes                  = Bytes.subspan(3);

    // 0x0F variant(0x38, 0x3A), XX
    return OpcodeResult{.Bytes = Map == OPCODE_MAP2_SELECT ? BuildOpcodeMap2(Opcode) : BuildOpcodeMap3(Opcode), .Length = 3};
  }

  assert(Bytes.size() >= 2 &&
         "opcode contains OPCODE_ESCAPE bit so Bytes must need to be atleast "
         "2-bytes");

  // now here we know that there is opcode map and bytes[0] is opcode_escape
  // so opcode is 2-bytes store that and return Bytes.subspan(2)

  std::array<std::byte, 2> Opcodes = BuildTwoBytesOpcode(Bytes[1]);
  Bytes                            = Bytes.subspan(2);

  // 0x0F XX
  return OpcodeResult{.Bytes = {Opcodes[0], Opcodes[1], std::byte{0x0}}, .Length = 2};
}

ModRM Disassembler::ModRMScanner(const std::byte Byte)
{
  /*
    example: 11 001 100
             |   |   |
            mod  reg  r/m
  */

  const std::uint8_t Value = std::to_integer<std::uint8_t>(Byte);

  const std::uint8_t Mod = (Value & 0xC0) >> 6;
  const std::uint8_t Reg = (Value & 0x38) >> 3;
  const std::uint8_t Rm  = Value & 0x07;

  return ModRM{.Mod = Mod, .Reg = Reg, .Rm = Rm};
}

ModRMOperandInfo Disassembler::ResolveModRM(const ModRM& ModRm, const InstructionDesc& MetaData)
{
  struct ModRMOperandInfo Info{};

  // todo: replace operand size with REX.W prefix check
  Info.Rm  = ResolveRegister(ModRm.Rm, OperandSize::Bits64);
  Info.Reg = ResolveRegister(ModRm.Reg, OperandSize::Bits64);

  if(MetaData.Destination == OperandCode::Ev && MetaData.Source == OperandCode::Gv)
  {
    Info.Destination = ModRMField::Rm;
    Info.Source      = ModRMField::Reg;
  }
  else if(MetaData.Destination == OperandCode::Gv && MetaData.Source == OperandCode::Ev)
  {
    Info.Destination = ModRMField::Reg;
    Info.Source      = ModRMField::Rm;
  }
  else
  {
    // generic fallback for now
    Info.Destination = ModRMField::Reg;
    Info.Source      = ModRMField::Rm;
  }

  switch(static_cast<ModRMMode>(ModRm.Mod))
  {
    case ModRMMode::Register:
      Info.Mode = ModRMMode::Register;
      break;

    case ModRMMode::MemoryNoDisplacement:
      Info.Mode = ModRMMode::MemoryNoDisplacement;
      break;

    case ModRMMode::MemoryDisp8:
      Info.Mode = ModRMMode::MemoryDisp8;
      break;

    case ModRMMode::MemoryDisp32:
      Info.Mode = ModRMMode::MemoryDisp32;
      break;

    default:
      std::println(stderr, "Invalid ModRM.mod");
      std::unreachable();
  }

  return Info;
}

SIB Disassembler::GetSibFromByte(std::uint8_t Byte, std::uint8_t mod)
{
  return SIB{
      .Scale = static_cast<std::int8_t>((Byte >> 6) & 0x03),
      .Index = static_cast<std::int8_t>(((Byte >> 3) & 0x07) == 0b100 ? -1 : (Byte >> 3) & 0x07),
      .Base  = static_cast<std::int8_t>(((Byte & 0x07) == 0b101 && mod == 0) ? -1 : Byte & 0x07),
  };
}

void Disassembler::BuildModRMInstruction(const ModRM& ModRm, const InstructionDesc& MetaData, std::span<const std::byte>& Bytes)
{
  ModRMOperandInfo Info     = ResolveModRM(ModRm, MetaData);
  std::string_view Mnemonic = ToString(MetaData.mnemonic);

  auto build_base_address = [&]() -> EffectiveAddress {
    EffectiveAddress effective_address{};

    // sib exists iff mod != register and rm = 100
    if(Info.Mode != ModRMMode::Register && ModRm.Rm == 4)
    {
      std::uint8_t sib_byte{};
      std::memcpy(&sib_byte, Bytes.data(), sizeof(sib_byte));
      Bytes                 = Bytes.subspan(sizeof(sib_byte));
      effective_address.sib = GetSibFromByte(sib_byte, ModRm.Mod);
    }

    if(Info.Mode == ModRMMode::MemoryDisp8)
    {
      std::int8_t disp{};
      std::memcpy(&disp, Bytes.data(), sizeof(std::int8_t));
      Bytes                          = Bytes.subspan(sizeof(std::int8_t));
      effective_address.displacement = disp;
    }
    else if(Info.Mode == ModRMMode::MemoryDisp32)
    {
      std::memcpy(&effective_address.displacement, Bytes.data(), sizeof(std::int32_t));
      Bytes = Bytes.subspan(sizeof(std::int32_t));
    }

    return effective_address;
  };

  std::string      SourceRegister      = Info.Source == ModRMField::Rm ? ToString(Info.Rm) : ToString(Info.Reg);
  std::string      DestinationRegister = Info.Destination == ModRMField::Rm ? ToString(Info.Rm) : ToString(Info.Reg);
  EffectiveAddress effective_address   = build_base_address();

  //std::println("{} {}, {}", Mnemonic, DestinationRegister,
  //             Info.Mode == ModRMMode::Register ?
  //                 SourceRegister :
  //                 effective_address.render_sib(OperandSize::Bits64, false, Image->NTHeaders, ModRm));
}

void Disassembler::build_immediate_instruction(const InstructionDesc& metadata, std::span<const std::byte>& bytes)
{
  if(metadata.Destination == OperandCode::Iv || metadata.Destination == OperandCode::Iz || metadata.Source == OperandCode::Ib
     || metadata.Source == OperandCode::Iv || metadata.Source == OperandCode::Iz || metadata.Source == OperandCode::Ib) [[likely]]
  {
    //std::println("imm: {:x}", std::to_integer<uint8_t>(bytes[0]));
  }
}
