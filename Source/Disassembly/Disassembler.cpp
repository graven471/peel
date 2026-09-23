#include "Disassembler.hpp"

#include <algorithm>
#include <cassert>
#include <print>
#include <ranges>
#include <span>

#include "x86-64/Opcodes.hpp"
#include "x86-64/Prefixes.hpp"
#include "x86-64/Instructions.hpp"

void Disassembler::DoIt()
{
  constexpr std::array TextName{'.', 't', 'e', 'x', 't', '\0', '\0', '\0'};

  auto It = std::ranges::find_if(Image->SectionHeaders, [&](const ImageSection& Section) {
    return std::ranges::equal(Section.Header.Name, TextName);
  });

  if(It == Image->SectionHeaders.end())
  {
    std::println(stderr, "no .text section is present");
    return;
  }

  std::println("found text section");

  ImageSection& TextSection = *It;

  std::span<const std::byte> InstructionEncoding = TextSection.Data.subspan(8, 32);

  // fetches the prefix if available and slides the InstructionEncoding to after prefix
  // e.g. if prefix is 2-bytes then InstructionEncoding will be subspan(2, ...)
  PrefixScanner(InstructionEncoding);
  // and slides the InstructionEncoding to either 1, 2, or 3 byte
  // todo: return opcode so i can send to metadata
  OpCodeScanner(InstructionEncoding);

  InstructionDesc OpcodeMetadata = OPCODE_TABLE[std::uint8_t{0X08}];

  if(OpcodeMetadata.ModRM)
  {
    // call modrm_scanner

    // disp
    // SIB etc
  }

  // immediate
}

void Disassembler::PrefixScanner(std::span<const std::byte> Bytes)
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
  auto PrefixEnd = std::ranges::find_if_not(TestBytesSpan, IsPrefix);

  // let PrefixEnd be &Data[N]
  // let begin() = &Data[0]
  // PrefixCount = N - 0 => N
  auto PrefixCount = PrefixEnd - TestBytesSpan.begin();

  // slide the instruction window should point to opcode now
  Bytes = Bytes.subspan(PrefixCount);

  // let PrefixCount = N, where N <= Data.size()
  // then Prefixes is a new view into Data from
  // assuming PrefixCount = PrefixEnd - Data.begin(), where Data.begin() == 0
  // [X.......N] where X represents some byte
  // first = give me first N elements of this span
  std::span<const std::byte> Prefixes = TestBytesSpan.first(PrefixCount);

  // let PrefixCount = N, where N <= Data.size()
  // then then Instruction is a new view into Data
  // from PrefixCount...Data.size()
  // conceptually
  // [N....Data.size()]
  //std::span<const std::byte> Rest = TestBytesSpan.subspan(PrefixCount);

  // todo: have some kind of structure or data that stores prefixes and group
  for(size_t I = 0; I < Prefixes.size(); I++)
  {
    std::print("Prefix: 0x{:02x} ", std::to_integer<uint8_t>(Prefixes[I]));
  }
}

// note some opcode requires ModR/M and some don't so i have return
// the info somehow that does opcode needs ModR/M or not
void Disassembler::OpCodeScanner(std::span<const std::byte> Bytes)
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
    // stop store opcode and return Bytes.subspan(1)
    std::println("there is no escape opcode");
    const std::byte Opcode = Bytes[0];
    std::println("opcode: 0x{:02x}", std::to_integer<std::uint8_t>(Opcode));
    Bytes = Bytes.subspan(1);
    return;
  }

  if(Bytes[1] == OPCODE_MAP2_SELECT || Bytes[1] == OPCODE_MAP3_SELECT)
  {
    assert(Bytes.size() >= 3 &&
           "opcode contains OPCODE_MAP byte so Bytes must need to be atleast "
           "3-bytes");
    std::println("found OPCODE_MAP2_SELECT or OPCODE_MAP3_SELECT");

    auto Opcodes = Bytes.first(3);
    Bytes        = Bytes.subspan(3);
    return;
  }

  assert(Bytes.size() >= 2 &&
         "opcode contains OPCODE_ESCAPE bit so Bytes must need to be atleast "
         "2-bytes");

  // now here we know that there is opcode map and bytes[0] is opcode_escape
  // so opcode is 2-bytes store that and return Bytes.subspan(2)

  auto Opcodes = Bytes.first(2);
  Bytes        = Bytes.subspan(2);

  std::array<std::byte, 2> Opcode = BuildTwoBytesOpcode(Opcodes[1]);

  for(auto& o : Opcode)
  {
    std::println("opcode: 0x{:02x}", std::to_integer<std::uint8_t>(o));
  }
}
