#include "pch.h"
#include "InstructionBuilder.h"

#ifdef IS32BIT
#define _BITSCAN_FLAGS (NONBYTE_GPR_ALLOWED | WORDPTR_ALLOWED | DWORDPTR_ALLOWED)
#else // IS64BIT
#define _BITSCAN_FLAGS (NONBYTE_GPR_ALLOWED | WORDPTR_ALLOWED | DWORDPTR_ALLOWED | QWORDPTR_ALLOWED)
#endif // IS64BIT

OpcodeBitScan::OpcodeBitScan(const String &mnemonic, UINT op)
  : Opcode2Arg(mnemonic, op, _BITSCAN_FLAGS)
{
}

bool OpcodeBitScan::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError) const {
  if(!validateIsRegisterOperand(op1, 1, throwOnError)) {
    return false;
  }
  if(!validateIsRegisterOrMemoryOperand(op2,2,throwOnError)) {
    return false;
  }
  if(!validateSameSize(op1, op2, throwOnError)) {
    return false;
  }
  return true;
}

InstructionBase OpcodeBitScan::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  isValidOperandCombination(op1,op2,true);
  switch(op2.getType()) {
  case REGISTER     : return InstructionBuilder(*this).setRegRegOperands(op1.getRegister(), op2.getRegister());
  case MEMORYOPERAND: return InstructionBuilder(*this).setMemoryRegOperands((MemoryOperand&)op2,op1.getRegister());
  }
  throwInvalidOperandCombination(op1,op2);
  return __super::operator()(op1,op2);
}
