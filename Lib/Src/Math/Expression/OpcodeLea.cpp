#include "pch.h"
#include "InstructionBuilder.h"

#define LEA_FLAGS (NONBYTE_GPR_ALLOWED | ALL_MEMOPSIZES | VOIDPTR_ALLOWED)

OpcodeLea::OpcodeLea(const String &mnemonic, BYTE op) : Opcode2Arg(mnemonic, op, LEA_FLAGS) {
}

bool OpcodeLea::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError) const {
  if(!validateRegisterOperand(op1, 1, throwOnError)) {
    return false;
  }
  if(!validateMemoryOperand(op2, 2, throwOnError)) {
    return false;
  }
  return true;
}

InstructionBase OpcodeLea::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  return __super::operator()(op1,VOIDPtr((MemoryOperand&)op2));
}
