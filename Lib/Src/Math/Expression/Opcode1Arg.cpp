#include "pch.h"
#include "InstructionBuilder.h"

class Instruction1Arg : public InstructionBuilder {
public:
  Instruction1Arg(const OpcodeBase &opcode) : InstructionBuilder(opcode) {
  }
  InstructionBuilder &setRegister(const Register &reg);
};

InstructionBuilder &Instruction1Arg::setRegister(const Register &reg) {
  const BYTE    regIndex = reg.getIndex();
  const RegSize regSize  = reg.getSize();
  switch(regSize) {
  case REGSIZE_BYTE :
    setModeBits(MR_REGREG(0,regIndex&7));
    break;
  case REGSIZE_WORD :
    wordIns();
    // continue case
  default           :
    setSizeBit().setModeBits(MR_REGREG(0,regIndex&7));
    break;
  }
  SETREXBITS(QWORDTOREX(regSize) | HIGHINDEXTOREX(regIndex,0));
  return *this;
}

InstructionBase Opcode1Arg::operator()(const InstructionOperand &op) const {
  isValidOperand(op, true);
  Instruction1Arg result(*this);
  switch(op.getType()) {
  case REGISTER       : return result.setRegister(op.getRegister());
  case MEMORYOPERAND  : return result.setMemoryOperand((MemoryOperand&)op);
  }
  return __super::operator()(op);
}
