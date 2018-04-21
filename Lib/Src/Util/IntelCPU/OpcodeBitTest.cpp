#include "pch.h"
#include "InstructionBuilder.h"

bool OpcodeBitTest::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError) const {
  if(op2.isImmediateValue()) {
    return m_immCode.isValidOperandCombination(op1,op2,throwOnError);
  } else {
    return __super::isValidOperandCombination(op1,op2,throwOnError);
  }
}

InstructionBase OpcodeBitTest::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  if(op2.isImmediateValue()) {
    return m_immCode(op1,op2);
  } else {
    return __super::operator()(op1,op2);
  }
}
