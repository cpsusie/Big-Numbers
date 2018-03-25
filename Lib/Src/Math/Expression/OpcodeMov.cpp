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
    or(8 | (regIndex&7)).add((BYTE*)&immv,2).wordIns();
    break;
  case REGSIZE_DWORD :
    if(!isDword(immv)) sizeError(method,reg,immv);
    or(8 | (regIndex&7)).add((BYTE*)&immv,4);
    break;
  case REGSIZE_QWORD :
    if(isDword(immv)) {
      xor(0x77).add(0xC0 | (regIndex&7)).add((BYTE*)&immv,4);
    } else {
      or(8 | (regIndex&7)).add((BYTE*)&immv,8);
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
    setMemoryOperand(dst).add((BYTE*)&immv,2);
    break;
  default:
    setMemoryOperand(dst).add((BYTE*)&immv,4);
    break;
  }
  return *this;
}

InstructionBase OpcodeMovRegImm::operator()(const Register &reg, const InstructionOperand &imm) const {
  assert(imm.getType() == IMMEDIATEVALUE);
  isValidOperandCombination(reg, imm, true);
  InstructionMovImm result(*this);
  return result.setGPRegImm((GPRegister&)reg, imm.getImmInt64());
}

#ifdef IS32BIT
static const RegSizeSet s_validImmSizeToRegMov(REGSIZE_BYTE, REGSIZE_WORD, REGSIZE_DWORD, REGSIZE_END);
#else // IS64BIT
static const RegSizeSet s_validImmSizeToRegMov(REGSIZE_BYTE, REGSIZE_WORD, REGSIZE_DWORD, REGSIZE_QWORD, REGSIZE_END);
#endif // IS64BIT

bool OpcodeMovRegImm::isValidOperandCombination(const Register &reg, const InstructionOperand &imm, bool throwOnError) const {
  assert(imm.getType() == IMMEDIATEVALUE);
  if(!validateRegisterAllowed(reg, throwOnError)) {
    return false;
  }
  if(!validateImmediateValue(reg.getSize(), imm, &s_validImmSizeToRegMov, throwOnError)) {
    return false;
  }
  return true;
}

InstructionBase OpcodeMovMemImm::operator()(const InstructionOperand &mem, const InstructionOperand &imm) const {
  assert((mem.getType() == MEMORYOPERAND) && (imm.getType() == IMMEDIATEVALUE));
  isValidOperandCombination(mem, imm, true);
  InstructionMovImm result(*this);
  return result.setMemImm((MemoryOperand&)mem, imm.getImmInt32());
}

InstructionBase OpcodeMov::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  if(op2.getType() != IMMEDIATEVALUE) {
    return __super::operator()(op1,op2);
  } else {
    switch(op1.getType()) {
    case REGISTER     : return m_regImmCode(op1.getRegister()  , op2);
    case MEMORYOPERAND: return m_memImmCode(op1, op2);
    default           : throwInvalidOperandCombination(__TFUNCTION__,op1,op2);
                        return __super::operator()(op1,op2); // should never come here
    }
  }
}

bool OpcodeMov::isValidOperandCombination(const Register &reg, const InstructionOperand &op, bool throwOnError) const {
  if(op.getType() == IMMEDIATEVALUE) {
    return m_regImmCode.isValidOperandCombination(reg,op, throwOnError);
  } else {
    return __super::isValidOperandCombination(reg,op, throwOnError);
  }
}
