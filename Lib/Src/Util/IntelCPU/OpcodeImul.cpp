#include "pch.h"
#include <OpCode.h>

bool OpcodeIMul::isValidOperandType(const InstructionOperand &op, BYTE index) const {
  validateOperandIndex(index);
  switch(index) {
  case 1: return isValidOperandType(op,index);
  case 2: return m_imul2ArgCode.isValidOperandType(op, index);
  case 3: return m_imul3ArgCode.isValidOperandType(op, index);
  }
  return false;
}

bool OpcodeIMul::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError) const {
  return m_imul2ArgCode.isValidOperandCombination(op1,op2,throwOnError);
}

bool OpcodeIMul::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3, bool throwOnError) const {
  return m_imul3ArgCode.isValidOperandCombination(op1,op2,op3,throwOnError);
}

InstructionBase OpcodeIMul::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  return m_imul2ArgCode(op1,op2);
}

InstructionBase OpcodeIMul::operator()(const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3) const {
  return m_imul3ArgCode(op1,op2,op3);
}
