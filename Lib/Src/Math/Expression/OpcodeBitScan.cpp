#include "pch.h"
#include "InstructionBuilder.h"

#define BITSCAN_FLAGS (NONBYTE_GPR_ALLOWED | NONBYTE_GPRPTR_ALLOWED | HAS_NONBYTE_SIZEBITS | FIRSTOP_REGONLY)

OpcodeBitScan::OpcodeBitScan(const String &mnemonic, UINT op)
  : Opcode2Arg(mnemonic, op, BITSCAN_FLAGS)
{
}
