#include "pch.h"
#include "InstructionBuilder.h"

class InstructionBitScan : public InstructionBuilder {
public:
  InstructionBitScan(const OpcodeBase &opcode) : InstructionBuilder(opcode) {
  }
  inline InstructionBuilder &setGPRegReg(const GPRegister &dst, const GPRegister &src) {
    return setRegRegOperands(dst, src);
  }
  inline InstructionBuilder &setGPRegMem(const GPRegister &dst, const MemoryOperand &src) {
    return setMemoryRegOperands(src,dst);
  }
};

bool OpcodeBitScan::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError) const {
  DEFINEMETHODNAME;
  switch(op1.getType()) {
  case REGISTER     :
    if(!validateRegisterAllowed(op1.getRegister(), throwOnError)) {
      return false;
    }
    break;
  case MEMORYOPERAND:
  case IMMEDIATEVALUE:
    if(!throwOnError) return false;
    throwInvalidOperandType(op1,1);
  default:
    throwUnknownOperandType(method,op1.getType());
  }
  switch(op2.getType()) {
  case REGISTER     :
    if(!validateRegisterAllowed(op2.getRegister(), throwOnError)) {
      return false;
    }
    break;
  case MEMORYOPERAND:
    if(!validateMemoryOperandAllowed((MemoryOperand&)op2, throwOnError)) {
      return false;
    }
    break;
  case IMMEDIATEVALUE:
    if(!throwOnError) return false;
    throwInvalidOperandType(op2,2);
  default:
    throwUnknownOperandType(method,op2.getType());
  }
  if(!validateSameSize(op1, op2, throwOnError)) {
    return false;
  }
  return true;
}

InstructionBase OpcodeBitScan::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  isValidOperandCombination(op1,op2,true);
  switch(op2.getType()) {
  case REGISTER     : return InstructionBitScan(*this).setGPRegReg((GPRegister&)op1.getRegister(), (GPRegister   &)op2.getRegister());
  case MEMORYOPERAND: return InstructionBitScan(*this).setGPRegMem((GPRegister&)op1.getRegister(), (MemoryOperand&)op2);
  }
  throwInvalidOperandCombination(op1,op2);
  return InstructionBitScan(*this);
}
