#include "pch.h"
#include "InstructionBuilder.h"

class Instruction1Arg : public InstructionBuilder {
public:
  Instruction1Arg(const OpcodeBase &opcode) : InstructionBuilder(opcode)
  {
  }
  InstructionBuilder &addMemoryReference(const MemoryOperand &mop) {
    if(needSizeBit(mop.getSize())) setSizeBit();
    return __super::addMemoryOperand(mop);
  }
  InstructionBuilder &setRegister(       const Register      &reg);
};

InstructionBuilder &Instruction1Arg::setRegister(const Register &reg) {
  const BYTE regIndex = reg.getIndex();
  SETREXBITONHIGHINX(regIndex,0);
  if(needSizeBit(reg.getSize())) setSizeBit();
  return add(0xc0 | (regIndex&7));
}

InstructionBase Opcode1Arg::operator()(const InstructionOperand &op) const {
  validateOpCount(1);
  Instruction1Arg result(*this);
  switch(op.getType()) {
  case REGISTER       :
    validateRegisterAllowed(op.getRegister());
    return result.setRegister(op.getRegister());
  case MEMORYOPERAND  :
    validateMemoryOperandAllowed((MemoryOperand&)op);
    return result.addMemoryReference((MemoryOperand&)op);
  case IMMEDIATEVALUE :
    validateImmediateValueAllowed();
    throwException(_T("%s:Immediate value not yet implemented. (op=%s)"), __TFUNCTION__,op.toString().cstr());
    break;
  }
  return result;
}
