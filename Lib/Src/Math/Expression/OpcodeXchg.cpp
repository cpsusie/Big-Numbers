#include "pch.h"
#include "InstructionBuilder.h"

static const RegSizeSet eaxRaxRegisterSizeSet(REGSIZE_DWORD ,REGSIZE_QWORD, REGSIZE_END);

static inline bool isEaxOrRAX(const Register &reg) {
  return reg.isGPR0() && eaxRaxRegisterSizeSet.contains(reg.getSize());
}

InstructionBase OpcodeXchg::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  if((op1.getType() == REGISTER) && (op2.getType() == REGISTER)) {
    const Register &reg1 = op1.getRegister();
    const Register &reg2 = op2.getRegister();
    if(isEaxOrRAX(reg1)) {
      validateRegisterOperand(op2,2,true);
      validateSameSize(reg1,reg2,true);
      return InstructionBuilder(m_eaxRegCode).setRegisterOperandNoModeByte((GPRegister&)reg2);
    } else if(isEaxOrRAX(reg2)) {
      validateRegisterOperand(op1,1,true);
      validateSameSize(reg1,reg2,true);
      return InstructionBuilder(m_eaxRegCode).setRegisterOperandNoModeByte((GPRegister&)reg1);
    }
  }
  return __super::operator()(op1,op2);
}
