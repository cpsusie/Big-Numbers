#include "pch.h"
#include "InstructionBuilder.h"

bool OpcodeFPUTransfer::isValidOperand(const InstructionOperand &op, bool throwOnError) const {
  switch(op.getType()) {
  case REGISTER     : return __super::isValidOperand(op,throwOnError);
  case MEMORYOPERAND:
    switch(op.getSize()) {
    case REGSIZE_DWORD: return m_dwordCode.isValidOperand(op,throwOnError);
    case REGSIZE_QWORD: return m_qwordCode.isValidOperand(op,throwOnError);
    case REGSIZE_TBYTE: return m_tbyteCode.isValidOperand(op,throwOnError);
    }
  }
  return __super::isValidOperand(op,throwOnError);
}

InstructionBase OpcodeFPUTransfer::operator()(const InstructionOperand &op) const {
  isValidOperand(op, true);
  switch(op.getType()) {
  case REGISTER       : return InstructionBuilder(*this).setRegisterOperand(op.getRegister());
  case MEMORYOPERAND  :
    switch(op.getSize()) {
    case REGSIZE_DWORD: return InstructionBuilder(m_dwordCode).setMemoryOperand((MemoryOperand&)op);
    case REGSIZE_QWORD: return InstructionBuilder(m_qwordCode).setMemoryOperand((MemoryOperand&)op);
    case REGSIZE_TBYTE: return InstructionBuilder(m_tbyteCode).setMemoryOperand((MemoryOperand&)op);
    }
  }
  return __super::operator()(op);
}
