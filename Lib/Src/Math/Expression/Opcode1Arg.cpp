#include "pch.h"
#include "InstructionBuilder.h"

InstructionBase Opcode1Arg::operator()(const InstructionOperand &op) const {
  isValidOperand(op, true);
  InstructionBuilder result(*this);
  switch(op.getType()) {
  case REGISTER       : return result.setRegisterOperand((GPRegister&   )op.getRegister());
  case MEMORYOPERAND  : return result.setMemoryOperand(  (MemoryOperand&)op);
  }
  return __super::operator()(op);
}
