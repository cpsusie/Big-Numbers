#include "pch.h"
#include "InstructionBuilder.h"

Opcode0Arg::Opcode0Arg(const String &mnemonic, const Opcode0Arg &op, OperandSize size)
  : OpcodeBase(mnemonic, InstructionBuilder(op).setOperandSize(size,true))
{
}
