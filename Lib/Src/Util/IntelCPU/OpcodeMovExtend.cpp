#include "pch.h"
#include "InstructionBuilder.h"

bool OpcodeMovExtend::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError) const {
  switch(op2.getSize()) {
  case REGSIZE_BYTE :
    return __super::isValidOperandCombination( op1,op2,throwOnError);
  case REGSIZE_WORD :
    return m_wCode.isValidOperandCombination(  op1,op2,throwOnError);
  case REGSIZE_DWORD:
    return m_dwCode.isValidOperandCombination( op1,op2,throwOnError);
  }
  return throwInvalidOperandCombination(op1,op2,throwOnError);
}

InstructionBase OpcodeMovExtend::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  switch(op2.getSize()) {
  case REGSIZE_BYTE : return __super::operator()(op1,op2);
  case REGSIZE_WORD : return m_wCode(op1,op2);
  case REGSIZE_DWORD: return m_dwCode(op1,op2);
  }
  return __super::operator()(op1,op2);
}
