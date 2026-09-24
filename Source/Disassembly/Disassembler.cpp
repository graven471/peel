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

  //std::span<const std::byte> InstructionEncoding = TextSection.Data.subspan(8, 32);

  std::span<const std::byte> InstructionEncoding = TestBytesSpan;

  // fetches the prefix if available and slides the InstructionEncoding to after prefix
  // e.g. if prefix is 2-bytes then InstructionEncoding will be subspan(2, ...)
  PrefixScanner(InstructionEncoding);
  OpcodeResult Opcode = OpcodeScanner(InstructionEncoding);

  // todo: abstract this into some metadata lookup function
  InstructionDesc OpcodeMetadata = OPCODE_TABLE[std::to_integer<std::uint8_t>(Opcode.Bytes[0])];

  std::println("{} {}, {}", ToString(OpcodeMetadata.mnemonic), ToString(OpcodeMetadata.Destination),
               ToString(OpcodeMetadata.Source));

  // todo: abstract this
  if(OpcodeMetadata.ModRM)
  {
    const std::byte ModRMByte = InstructionEncoding[0];
    std::println("ModR/M is 0x{:02x}", std::to_integer<uint8_t>(ModRMByte));

    ModRM ModRM = ModRMScanner(ModRMByte);
    // slide InstructionEncoding to point after ModR/M byte
    InstructionEncoding = InstructionEncoding.subspan(1);

    BuildModRMInstruction(ModRM, OpcodeMetadata);
  }
}

// the reason we need to pass reference here because even tho span is just ptr and size
// is that if we pass by value it will underlying ptr and size and construct a local span
// and modification will be on that local span instead of modifying Bytes which is in caller stack frame
// i thought since its a pointer they both will point to same data so i can modify it without reference
void Disassembler::PrefixScanner(std::span<const std::byte>& Bytes)
{
  // for I in instruction encoding:
  //   is I in legacy group 1-4 prefix ?
  //   is I in REX prefix ?
  //   if I is prefix then push I to somewhere (yet to be defined)
  //   go to next byte I + 1 check if its in prefix
  //   repeat until we found something that is not prefix
  //   if not stop and return subspan from there
  //   [2E 66 3E 48 8B 05 E7 33 03]
  //    ----------- |
  //    prefix      new subspan loc

  // PrefixEnd == pos where NOT isPrefix for example like take
  // [2E 66 3E 48 8B 05 E7 33 03]
  //  |---------|
  //    prefix    ^
  //               prefix_end

  // it will walk it through for each iteration it checks !IsPrefix(idx)
  // IsPrefix(2E) -> true -> continue
  // IsPrefix(66) -> true -> continue
  // IsPrefix(3E) -> true -> continue
  // IsPrefix(48) -> true -> continue
  // IsPrefix(8B) -> false -> stop
  // so it stops and gives the index in this case 4

  // PrefixEnd is an Iterator pointing to 4th element in span
  auto PrefixEnd = std::ranges::find_if_not(Bytes, IsPrefix);

  // let PrefixEnd be &Data[N]
  // let begin() = &Data[0]
  // PrefixCount = N - 0 => N
  auto PrefixCount = PrefixEnd - Bytes.begin();

  // let PrefixCount = N, where N <= Data.size()
  // then Prefixes is a new view into Data from
  // assuming PrefixCount = PrefixEnd - Data.begin(), where Data.begin() == 0
  // [X.......N] where X represents some byte
  // first = give me first N elements of this span
  std::span<const std::byte> Prefixes = Bytes.first(PrefixCount);

  // let PrefixCount = N, where N <= Data.size()
  // then then Instruction is a new view into Data
  // from PrefixCount...Data.size()
  // conceptually
  // [N....Data.size()]
  //std::span<const std::byte> Rest = TestBytesSpan.subspan(PrefixCount);

  // slide the instruction window should point to opcode now
  Bytes = Bytes.subspan(PrefixCount);

  // todo: have some kind of structure or data that stores prefixes and group
  for(size_t I = 0; I < Prefixes.size(); I++)
  {
    std::print("Prefix: 0x{:02x} ", std::to_integer<uint8_t>(Prefixes[I]));
  }
}

// note some opcode requires ModR/M and some don't so i have return
// the info somehow that does opcode needs ModR/M or not
OpcodeResult Disassembler::OpcodeScanner(std::span<const std::byte>& Bytes)
{
  // we are assuming that the Bytes[0] will be valid x86-64 opcode
  // opcode can be either 1 byte or 2 bytes or 3 bytes
  // an additional 3-bit opcode field is sometimes encoded in the ModR/M byte

  // if opcode is 2-bytes then:
  // important values are: 0x0F escape opcode byte as the primary opcode and a
  // second opcode byte.
  // mandatory prefix: (0x66, 0xF2, 0xF3), an escape opcode byte, and a second
  // opcode byte

  // For example, CVTDQ2PD consists of the following sequence: F3 0F E6. The
  // first byte is a mandatory prefix (it is not considered as a repeat prefix).

  // so if first opcode byte is 0xF then the opcode is just 1-byte
  // otherwise i have to read another byte

  // if opcode is 3-bytes then:
  // an escape opcode byte is 0xF as the primary opcode, plus two additional
  // opcode bytes.
  // a mandatory prefix (0x66, 0xF2, or 0xF3), an escape opcode byte

  // For example, PHADDW for XMM registers consists of the following sequence:
  // 0x66 0x0F 0x38 0x01. The first byte is the mandatory prefix.

  // escape opcodes are 0x0F, 0x0F 0x38

  // implementation

  /*
    if opcode[0] = 0x0F then read another opcode[1]
    MAP = 0x0F xx
    else stop and opcode_length = 1

    if opcode[1] = 0x38 or 0x3A then read opcode[2]
       if 0x38 then MAP = 0x0F 0x38 xx
       else if 0x3A then MAP = 0x0F 0x3A xx
    else stop opcode_length = 2
  */

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

  mod -> defines how we should treat r/m bits the values are:

  11 -> treat r/m as general purpose register
  00 -> treat r/m as memory address
  01 -> then assuming AMD64 mode treat r/m as [base_register + disp8]
  10 -> then assuming AMD64 mode treat r/m as [base_register + disp32]
  
  mod = 00, r/m = 101 is special one in AMD64 it means treat r/m as [RIP + disp32]

  reg -> note some modr/m can have opcode extension instead of /r something like /1 .. /7
  but assuming /r this maps to general purpose registers and registers width depends on architecture
  000 -> AX, EAX, RAX
  001 -> CX, ECX, RCX
  ...
  111 -> DI, EDI, RDI

  example: Byte = 1100 1000 (0xC8)

  mod = 11  ->  register access
  reg = 001 -> RCX,EDX etc depends on architecture
  r/m = 000 -> and since mod = 11 we should treat this as register so RAX,EAX etc


  so how to translate this in code

  i want:

  uint8_t mod = 11
  uint8_t reg = 001
  uint8_t rm = 000

  `w = x - n + 1` where x is the highest bit and n is the lowest bit

  mask = ((1 <= w) - 1) << n, where w = ones

  field = (Byte & mask) >= n

  mod: x = 7, n = 6
  mask = ((1 << 2) - 1) << 6
       = 0b11 << 6
       = 1100 0000 (0xC0)

   reg: x=5, n=3
   mask = ((1 << 3) - 1) << 3
     = 0b111 << 3
     = 0011 1000 (0x38)

  r/m: x=2, n=0
  mask = ((1 << 3) - 1) << 0
       = 0000 0111 (0x7)
  */

  const std::uint8_t Value = std::to_integer<std::uint8_t>(Byte);

  const std::uint8_t Mod = (Value & 0xC0) >> 6;
  const std::uint8_t Reg = (Value & 0x38) >> 3;
  const std::uint8_t Rm  = Value & 0x07;

  std::println("Mod: 0x{:02x}, Reg: 0x{:02x}, Rm: 0x{:02x}", Mod, Reg, Rm);

  return ModRM{.Mod = Mod, .Reg = Reg, .Rm = Rm};
}

ModRMOperandInfo Disassembler::ResolveModRM(const ModRM& ModRm, const InstructionDesc& MetaData)
{
  struct ModRMOperandInfo Info{};

  Info.Rm  = ResolveRegister(ModRm.Rm, OperandSize::Bits32);
  Info.Reg = ResolveRegister(ModRm.Reg, OperandSize::Bits32);

  if(MetaData.Destination == OperandCode::Ev && MetaData.Source == OperandCode::Gv)
  {
    Info.Destination = ModRMField::Rm;
    Info.Source      = ModRMField::Reg;
  }

  // generic fallback for now
  Info.Destination = ModRMField::Reg;
  Info.Source      = ModRMField::Rm;

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

void Disassembler::BuildModRMInstruction(const ModRM& ModRm, const InstructionDesc& MetaData)
{
  ModRMOperandInfo Info     = ResolveModRM(ModRm, MetaData);
  std::string_view Mnemonic = ToString(MetaData.mnemonic);

  if(Info.Mode == ModRMMode::Register || Info.Mode == ModRMMode::MemoryNoDisplacement)
  {
    std::string_view SourceRegister      = Info.Destination == ModRMField::Rm ? ToString(Info.Rm) : ToString(Info.Reg);
    std::string_view DestinationRegister = Info.Destination == ModRMField::Rm ? ToString(Info.Rm) : ToString(Info.Reg);

    auto Format = Info.Mode == ModRMMode::MemoryNoDisplacement ?
                      std::format("{} {}, [{}]", Mnemonic, DestinationRegister, SourceRegister) :
                      std::format("{} {}, {}", Mnemonic, DestinationRegister, SourceRegister);

    std::println("{}", Format);
  }
}
