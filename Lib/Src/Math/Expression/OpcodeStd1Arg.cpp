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
  const BYTE regIndex = reg.getIndex();
  SETREXBITONHIGHINX(regIndex,0);
  return add(0xc0 | (regIndex&7));
}

InstructionBase Opcode1Arg::operator()(const InstructionOperand &op) const {
  validateOpCount(1);
  InstructionStd1Arg result(*this);
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
