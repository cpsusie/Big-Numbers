#include "pch.h"

namespace IntelCPU {

bool OpcodeLea::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError) const {
  if(!validateIsRegisterOperand(op1, 1, throwOnError)) {
    return false;
  }
  if(!validateIsMemoryOperand(op2, 2, throwOnError)) {
    return false;
  }
  return true;
}

InstructionBase OpcodeLea::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  return __super::operator()(op1,VOIDPtr((MemoryOperand&)op2));
}

}; // namespace
