#include "pch.h"
#include "InstructionBuilder.h"

bool OpcodeTest::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError) const {
  if(op2.getType() != IMMEDIATEVALUE) {
    return __super::isValidOperandCombination(op1,op2,throwOnError);
  } else {
    if(op1.isGPR0()) {
      return m_GPR0ImmCode.isValidOperandCombination(op1,op2,throwOnError);
    } else {
      return m_immCode.isValidOperandCombination(op1,op2,throwOnError);
    }
  }
}

InstructionBase OpcodeTest::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  if(op2.getType() != IMMEDIATEVALUE) {
    return __super::operator()(op1, op2);
  } else {
    isValidOperandCombination(op1,op2,true);
    if(op1.isGPR0()) {
      return InstructionBuilder(m_GPR0ImmCode).setOperandSize(op1.getSize()).addImmediateOperand(op2,op1.getLimitedSize(REGSIZE_DWORD));
    } else {
      return InstructionBuilder(m_immCode).setMemOrRegOperand(op1).addImmediateOperand(op2,op1.getLimitedSize(REGSIZE_DWORD));
    }
  }
  throwInvalidOperandCombination(op1, op2);
  return __super::operator()(op1, op2); // should never come here
}
