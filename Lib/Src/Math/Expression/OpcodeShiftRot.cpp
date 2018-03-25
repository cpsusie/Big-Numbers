#include "pch.h"
#include "InstructionBuilder.h"

class InstructionShiftRot : public InstructionBuilder {
public:
  InstructionShiftRot(const OpcodeBase &opcode) : InstructionBuilder(opcode)
  {
  }
  inline InstructionBuilder &setGPRegCL(const GPRegister    &reg) {
    return setRegisterOperand(reg);
  }
  inline InstructionBuilder &setMemCL(   const MemoryOperand &mem) {
    return setMemoryOperand(mem);
  }
  inline InstructionBuilder &setGPRegImm(const GPRegister    &reg, BYTE immv) {
    return setRegisterOperand(reg).add(immv);
  }
  inline InstructionBuilder &setMemImm(  const MemoryOperand &mem, BYTE immv) {
    return setMemoryOperand(mem).add(immv);
  }
};

bool OpcodeShiftRot::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError) const {
  DEFINEMETHODNAME;
  switch(op1.getType()) {
  case REGISTER     :
    if(!validateRegisterAllowed(op1.getRegister(), throwOnError)) {
      return false;
    }
    break;
  case MEMORYOPERAND:
    if(!validateMemoryOperandAllowed((MemoryOperand&)op1, throwOnError)) {
      return false;
    }
    break;
  case IMMEDIATEVALUE:
    if(!throwOnError) return false;
    throwInvalidOperandType(op1,1);
  default:
    throwUnknownOperandType(method,op1.getType());
  }
  switch(op2.getType()) {
  case REGISTER      :
    if(op2.getRegister() != CL) {
      if(!throwOnError) return false;
      throwInvalidArgumentException(method, _T("Register=%s. Must be cl"), op2.toString().cstr());
    }
    break;
  case MEMORYOPERAND :
    if(!throwOnError) return false;
    throwInvalidOperandType(op2,2);
  case IMMEDIATEVALUE:
    if(op2.getSize() != REGSIZE_BYTE) {
      if(!throwOnError) return false;
      throwInvalidArgumentException(method, _T("Immediate value must be BYTE"));
    }
    break;
  default:
    throwUnknownOperandType(method,op2.getType());
  }
  return true;
}

InstructionBase OpcodeShiftRot::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  isValidOperandCombination(op1, op2, true);
  switch(op2.getType()) {
  case REGISTER      :
    switch(op1.getType()) {
    case REGISTER     : return InstructionShiftRot(*this).setGPRegCL((GPRegister   &)op1.getRegister());
    case MEMORYOPERAND: return InstructionShiftRot(*this).setMemCL(  (MemoryOperand&)op1              );
    }
    break;
  case IMMEDIATEVALUE:
    switch(op1.getType()) {
    case REGISTER     : return InstructionShiftRot(m_immCode).setGPRegImm((GPRegister   &)op1.getRegister(),op2.getImmInt8());
    case MEMORYOPERAND: return InstructionShiftRot(m_immCode).setMemImm(  (MemoryOperand&)op1              ,op2.getImmInt8());
    }
    break;
  }
  throwInvalidOperandCombination(op1,op2);
  return __super::operator()(op1,op2);
}
