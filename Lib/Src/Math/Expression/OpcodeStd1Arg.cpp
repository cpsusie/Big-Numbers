#include "pch.h"
#include <Math/Expression/NewOpCode.h>
#include "RexByte.h"

class InstructionStd1Arg : public InstructionBuilder {
public:
  InstructionStd1Arg(const OpcodeBase &opcode) : InstructionBuilder(opcode)
  {
  }
  InstructionBase &addMemoryReference(const MemoryOperand &mop) {
    return __super::addMemoryReference(mop);
  }
  InstructionBase &setRegister(       const Register      &reg);
};

InstructionBase &InstructionStd1Arg::setRegister(const Register &reg) {
  SETREXBITONHIGHREG(reg,0);
  return add(0xc0 | (reg.getIndex()&7));
}

InstructionBase OpcodeStd1Arg::operator()(const InstructionOperand &op) const {
  validateOpCount(1);
  InstructionStd1Arg result(*this);
  switch(op.getType()) {
  case REGISTER       :
    validateRegisterAllowed(op.getRegister());
    return result.setRegister(op.getRegister());
  case MEMREFERENCE   :
    validateMemoryReferenceAllowed();
    validateOperandSize(op.getSize());
    return result.addMemoryReference((MemoryOperand&)op);
  case IMMEDIATEVALUE :
    validateImmediateValueAllowed();
    throwException(_T("%s:Immediate value not yet implemented. (op=%s)"), __TFUNCTION__,op.toString().cstr());
    break;
  }
  return result;
}
