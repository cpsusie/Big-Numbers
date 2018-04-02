#include "pch.h"
#include "InstructionBuilder.h"

OpcodeShiftRot::OpcodeShiftRot(const String &mnemonic, BYTE extension)
  : OpcodeBase(mnemonic, 0xD2, extension, 2, ALL_GPR_ALLOWED | ALL_GPRPTR_ALLOWED | IMM8_ALLOWED | HAS_ALL_SIZEBITS)
  , m_immCode( mnemonic, 0xC0, extension, 2, ALL_GPR_ALLOWED | ALL_GPRPTR_ALLOWED | IMM8_ALLOWED | HAS_ALL_SIZEBITS)
{
}

bool OpcodeShiftRot::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError) const {
  if(!validateIsRegisterOrMemoryOperand(op1, 1, throwOnError)) {
    return false;
  }
  return validateIsShiftAmountOperand(op2, 2, throwOnError);
}

InstructionBase OpcodeShiftRot::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  isValidOperandCombination(op1, op2, true);
  switch(op2.getType()) {
  case REGISTER      :
    switch(op1.getType()) {
    case REGISTER     : return InstructionBuilder(*this    ).setRegisterOperand((GPRegister   &)op1.getRegister());
    case MEMORYOPERAND: return InstructionBuilder(*this    ).setMemoryOperand(  (MemoryOperand&)op1              );
    }
    break;
  case IMMEDIATEVALUE:
    switch(op1.getType()) {
    case REGISTER     : return InstructionBuilder(m_immCode).setRegisterOperand((GPRegister   &)op1.getRegister()).add(op2.getImmInt8());
    case MEMORYOPERAND: return InstructionBuilder(m_immCode).setMemoryOperand(  (MemoryOperand&)op1              ).add(op2.getImmInt8());
    }
    break;
  }
  throwInvalidOperandCombination(op1,op2);
  return __super::operator()(op1,op2);
}

#define _DSHIFT_FLAGS (NONBYTE_GPR_ALLOWED | NONBYTE_GPRPTR_ALLOWED | IMM8_ALLOWED | HAS_NONBYTE_SIZEBITS)

OpcodeDoubleShift::OpcodeDoubleShift(const String &mnemonic, UINT opCL, UINT opImm)
  : OpcodeBase(mnemonic, opCL , 0, 3, _DSHIFT_FLAGS)
  , m_immCode( mnemonic, opImm, 0, 3, _DSHIFT_FLAGS)
{
}

bool OpcodeDoubleShift::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3, bool throwOnError) const {
  if(!validateIsRegisterOrMemoryOperand(op1, 1, throwOnError)) {
    return false;
  }
  if(!validateIsRegisterOperand(op2, 2, throwOnError)) {
    return false;
  }
  if(!validateSameSize(op1, op2, throwOnError)) {
    return false;
  }
  return validateIsShiftAmountOperand(op3,3,throwOnError);
}

InstructionBase OpcodeDoubleShift::operator()(const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3) const {
  isValidOperandCombination(op1, op2, op3, true);
  switch(op3.getType()) {
  case REGISTER      :
    switch(op1.getType()) {
    case REGISTER     : return InstructionBuilder(*this    ).setRegRegOperands(    op2.getRegister() , op1.getRegister());
    case MEMORYOPERAND: return InstructionBuilder(*this    ).setMemoryRegOperands((MemoryOperand&)op1, op2.getRegister());
    }
    break;
  case IMMEDIATEVALUE:
    switch(op1.getType()) {
    case REGISTER     : return InstructionBuilder(m_immCode).setRegRegOperands(   op2.getRegister()  , op1.getRegister()).add(op3.getImmInt8());
    case MEMORYOPERAND: return InstructionBuilder(m_immCode).setMemoryRegOperands((MemoryOperand&)op1, op2.getRegister()).add(op3.getImmInt8());
    }
    break;
  }
  throwInvalidOperandCombination(op1,op2,op3);
  return __super::operator()(op1,op2,op3);
}
