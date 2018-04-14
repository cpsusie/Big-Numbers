#include "pch.h"
#include <NewOpCode.h>
#include "InstructionBuilder.h"

bool OpcodeCall::isValidOperand(const InstructionOperand &op, bool throwOnError) const {
  switch(op.getType()) {
  case REGISTER      :
  case MEMORYOPERAND : return __super::isValidOperand(        op, throwOnError);
  case IMMEDIATEVALUE: return m_callNearRelImm.isValidOperand(op, throwOnError);
  }
  throwUnknownOperandType(op,1);
  return false;
}

InstructionBase OpcodeCall::operator()(const InstructionOperand &op) const {
  switch(op.getType()) {
  case REGISTER      :
  case MEMORYOPERAND :
    return __super::operator()(op);
  case IMMEDIATEVALUE:
    m_callNearRelImm.isValidOperand(op,true);
    switch(op.getSize()) {
    case REGSIZE_BYTE :
    case REGSIZE_WORD :
    case REGSIZE_DWORD:
      return InstructionBuilder(m_callNearRelImm).addImmediateOperand(op,REGSIZE_DWORD);
    }
  }
  throwUnknownOperandType(op,1);
  return __super::operator()(op);
}
