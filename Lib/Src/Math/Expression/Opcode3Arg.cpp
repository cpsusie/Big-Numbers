#include "pch.h"
#include "InstructionBuilder.h"

InstructionBase Opcode3Arg::operator()(const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3) const {
  isValidOperandCombination(op1,op2,op3,true);
  if(op3.getType() == IMMEDIATEVALUE) {
    switch(op1.getType()) {
    case REGISTER       :
      switch(op2.getType()) {
      case REGISTER       : // reg,reg,imm
        return InstructionBuilder(*this).setRegRegOperands(op1.getRegister(),op2.getRegister()).setImmediateOperand(op3, &op1);
      case MEMORYOPERAND  : // reg,mem,imm
        return InstructionBuilder(*this).setMemoryRegOperands((MemoryOperand&)op2, op1.getRegister()).setImmediateOperand(op3, &op1);
      case IMMEDIATEVALUE : // reg,imm,imm : ERROR
        break;
      }
      break;
    case MEMORYOPERAND    :
      break;
    }
  }
  throwInvalidOperandCombination(op1,op2,op3);
  return __super::operator()(op1,op2,op3);
}
