#include "pch.h"
#include "InstructionBuilder.h"

static const RegSizeSet eaxRaxRegisterSizeSet(REGSIZE_DWORD ,REGSIZE_QWORD, REGSIZE_END);

static inline bool isEaxOrRAX(const Register &reg) {
  return (reg.getType() == REGTYPE_GPR) && (reg.getIndex() == 0) && eaxRaxRegisterSizeSet.contains(reg.getSize());
}

class InstructionXchgEAX : public InstructionBuilder {
public:
  InstructionXchgEAX(const OpcodeBase &opcode) : InstructionBuilder(opcode) {
  }
  InstructionBuilder &setReg(const Register &reg);
};

InstructionBuilder &InstructionXchgEAX::setReg(const Register &reg) {
  const BYTE regIndex = reg.getIndex();
  switch(reg.getSize()) {
  case REGSIZE_QWORD :
    SETREXBITS(QWORDTOREX(REGSIZE_QWORD));
    // continue case
  case REGSIZE_DWORD :
    or(regIndex&7);
    break;
  }
  SETREXBITONHIGHINX(regIndex,0);
  return *this;
}

InstructionBase OpcodeXchg::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  if((op1.getType() == REGISTER) && (op2.getType() == REGISTER)) {
    const Register &reg1 = op1.getRegister();
    const Register &reg2 = op2.getRegister();
    if(isEaxOrRAX(reg1)) {
      validateRegisterOperand(op2,2,true);
      validateSameSize(reg1,reg2,true);
      return InstructionXchgEAX(m_eaxRegCode).setReg(reg2);
    } else if(isEaxOrRAX(reg2)) {
      validateRegisterOperand(op1,1,true);
      validateSameSize(reg1,reg2,true);
      return InstructionXchgEAX(m_eaxRegCode).setReg(reg1);
    }
  }
  return __super::operator()(op1,op2);
}
