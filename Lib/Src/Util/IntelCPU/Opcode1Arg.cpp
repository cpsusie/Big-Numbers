#include "pch.h"
#include <NewOpCode.h>
#include "InstructionBuilder.h"

InstructionBase Opcode1Arg::operator()(const InstructionOperand &op) const {
  isValidOperand(op, true);
  switch(op.getType()) {
  case REGISTER       :
  case MEMORYOPERAND  : return InstructionBuilder(*this).setMemOrRegOperand(op);
  case IMMEDIATEVALUE : return InstructionBuilder(*this).setImmediateOperand(op);
  }
  return __super::operator()(op);
}

#ifdef IS32BIT
InstructionBase OpcodeIncDec::operator()(const InstructionOperand &op) const {
  isValidOperand(op, true);
  if((op.getType() == REGISTER) && (op.getSize() == REGSIZE_DWORD)) {
    return InstructionBuilder(m_opReg32).or(op.getRegister().getIndex()&7);
  }
  return __super::operator()(op);
}
#endif // IS32BIT
