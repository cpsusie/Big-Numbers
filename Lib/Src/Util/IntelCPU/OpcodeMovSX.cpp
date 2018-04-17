#include "pch.h"
#include "InstructionBuilder.h"

InstructionBase OpcodeMovSX::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  switch(op2.getSize()) {
  case REGSIZE_BYTE :
    return __super::operator()(op1,op2);
  case REGSIZE_WORD :
    return m_sxwCode(op1,op2);
  case REGSIZE_DWORD:
    return m_sxdCode(op1,op2);
  }
  throwInvalidOperandCombination(op1,op2);
  return __super::operator()(op1,op2);
}
