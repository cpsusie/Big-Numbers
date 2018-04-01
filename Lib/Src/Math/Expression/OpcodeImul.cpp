#include "pch.h"
#include "InstructionBuilder.h"

bool OpcodeIMul::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError) const {
  return false;
}

bool OpcodeIMul::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3, bool throwOnError) const {
  return false;
}

InstructionBase OpcodeIMul::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  return InstructionBuilder(*this);
}

InstructionBase OpcodeIMul::operator()(const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3) const {
  return InstructionBuilder(*this);
}
