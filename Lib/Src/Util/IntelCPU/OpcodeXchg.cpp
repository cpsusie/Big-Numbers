#include "pch.h"
#include "InstructionBuilder.h"

static inline bool isEAXOrRAX(const Register &reg) {
  return reg.isGPR0() && Register::sizeContainsSrcSize(reg.getSize(),REGSIZE_DWORD);
}

InstructionBase OpcodeXchg::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  if(op1.isRegister() && op2.isRegister()) {
    const Register &reg1 = op1.getRegister();
    const Register &reg2 = op2.getRegister();
    if(isEAXOrRAX(reg1)) {
      validateIsRegisterOperand(op2,2,true);
      validateCompatibleSize(reg1,reg2,true);
      return m_eaxRegCode(reg2);
    } else if(isEAXOrRAX(reg2)) {
      validateIsRegisterOperand(op1,1,true);
      validateCompatibleSize(reg1,reg2,true);
      return m_eaxRegCode(reg1);
    }
  }
  return __super::operator()(op1,op2);
}
