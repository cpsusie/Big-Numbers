#include "pch.h"
#include "InstructionBuilder.h"

class InstructionMovImm : public InstructionBuilder {
public:
  InstructionMovImm(const OpcodeBase &opcode) : InstructionBuilder(opcode)
  {
  }
  InstructionBuilder &setGPRegImm(const GPRegister    &dst, INT64 immv);
  InstructionBuilder &setMemImm(  const MemoryOperand &dst, int   immv);
};

InstructionBuilder &InstructionMovImm::setGPRegImm(const GPRegister &reg, INT64 immv) {
  DEFINEMETHODNAME;
  const BYTE    regIndex = reg.getIndex();
  const RegSize regSize  = reg.getSize();
  switch(regSize) {
  case REGSIZE_BYTE :
    if(!isByte(immv)) sizeError(method,reg,immv);
    or(regIndex&7).add((char)immv);
    break;
  case REGSIZE_WORD :
    if(!isWord(immv)) sizeError(method,reg,immv);
    or(8 | (regIndex&7)).add(immv,2).wordIns();
    break;
  case REGSIZE_DWORD :
    if(!isDword(immv)) sizeError(method,reg,immv);
    or(8 | (regIndex&7)).add(immv,4);
    break;
  case REGSIZE_QWORD :
    if(isDword(immv)) {
      xor(0x77).add(0xC0 | (regIndex&7)).add(immv,4);
    } else {
      or(8 | (regIndex&7)).add(immv,8);
    }
    break;
  }
  SETREXBITS(QWORDTOREX(regSize) | HIGHINDEXTOREX(regIndex,0));
  return *this;
}

InstructionBuilder &InstructionMovImm::setMemImm(const MemoryOperand &dst, int immv) {
  DEFINEMETHODNAME;
  const OperandSize size = dst.getSize();
  switch(size) {
  case REGSIZE_BYTE :
    if(!isByte(immv)) sizeError(method,dst,immv);
    setMemoryOperand(dst).add((char)immv);
    break;
  case REGSIZE_WORD :
    if(!isWord(immv)) sizeError(method,dst,immv);
    setMemoryOperand(dst).add(immv,2);
    break;
  default:
    setMemoryOperand(dst).add(immv,4);
    break;
  }
  return *this;
}

InstructionBase OpcodeMovRegImm::operator()(const Register &reg, const InstructionOperand &op) const {
  assert(op.getType() == IMMEDIATEVALUE);
  InstructionMovImm result(*this);
  validateRegisterAllowed(reg);
  VALIDATEISREXCOMPATIBLE(reg,op);
  return result.setGPRegImm((GPRegister&)reg, op.getImmInt64());
}

#ifdef IS32BIT
#define s_validImmSizeToRegMov s_validImmSizeToReg
#else // IS64BIT
static const RegSizeSet s_validImmSizeToRegMov(REGSIZE_BYTE, REGSIZE_WORD, REGSIZE_DWORD, REGSIZE_QWORD, REGSIZE_END);
#endif // IS64BIT

bool OpcodeMovRegImm::isValidOperandCombination(const Register &reg, const InstructionOperand &op) const {
  assert(op.getType() == IMMEDIATEVALUE);
  if(!isRegisterAllowed(reg)) return false;
  if(!isImmediateValueAllowed()) return false;
  IF_NOT_REXCOMPATIBLE_RETURN_FALSE(reg,false);
  return sizeContainsSrcSize(reg.getSize(), op.getSize()) && s_validImmSizeToRegMov.contains(op.getSize());
}

InstructionBase OpcodeMovMemImm::operator()(const MemoryOperand &memop, const InstructionOperand &op) const {
  assert(op.getType() == IMMEDIATEVALUE);
  InstructionMovImm result(*this);
  validateMemoryOperandAllowed(memop);
  return result.setMemImm(memop, op.getImmInt32());
}

InstructionBase OpcodeMov::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  if(op2.getType() != IMMEDIATEVALUE) {
    return __super::operator()(op1,op2);
  } else {
    switch(op1.getType()) {
    case REGISTER     : return m_regImmCode(op1.getRegister()  , op2);
    case MEMORYOPERAND: return m_memImmCode((MemoryOperand&)op1, op2);
    default           : throwInvalidOperandCombination(__TFUNCTION__,op1,op2);
                        return __super::operator()(op1,op2); // should never come here
    }
  }
}

bool OpcodeMov::isValidOperandCombination(const Register &reg, const InstructionOperand &op) const {
  if(op.getType() == IMMEDIATEVALUE) {
    return m_regImmCode.isValidOperandCombination(reg,op);
  } else {
    return __super::isValidOperandCombination(reg,op);
  }
}
