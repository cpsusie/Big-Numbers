#include "pch.h"
#include "InstructionBuilder.h"

bool OpcodePushPop::isValidOperand(const InstructionOperand &op, bool throwOnError) const {
  switch(op.getType()) {
  case REGISTER      : return __super::isValidOperand( op, throwOnError);
  case MEMORYOPERAND : return m_memCode.isValidOperand(op, throwOnError);
  case IMMEDIATEVALUE: return m_immCode.isValidOperand(op, throwOnError);
  default            : throwUnknownOperandType(op,1);
  }
  return false;
}

InstructionBase OpcodePushPop::operator()(const InstructionOperand &op) const {
  switch(op.getType()) {
  case REGISTER      :
    isValidOperand(op,true);
    return InstructionBuilder(*this).setRegisterOperandNoModeByte((GPRegister&)op.getRegister());
  case MEMORYOPERAND :
    return m_memCode(op);
  case IMMEDIATEVALUE:
    isValidOperand(op,true);
    return InstructionBuilder(m_immCode).setImmediateOperand(op);
  }
  throwUnknownOperandType(op,1);
  return InstructionBuilder(*this);
}
