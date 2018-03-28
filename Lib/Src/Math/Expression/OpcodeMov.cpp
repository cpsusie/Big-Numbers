#include "pch.h"
#include "InstructionBuilder.h"

class InstructionMovImm : public InstructionBuilder {
public:
  InstructionMovImm(const OpcodeBase &opcode) : InstructionBuilder(opcode) {
  }
  InstructionBuilder &setRegImm(const Register      &dst, INT64 immv);
  InstructionBuilder &setMemImm(const MemoryOperand &dst, int   immv);
};

InstructionBuilder &InstructionMovImm::setRegImm(const Register &reg, INT64 immv) {
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

#ifdef IS32BIT
static const RegSizeSet s_validImmSizeToRegMov(REGSIZE_BYTE, REGSIZE_WORD, REGSIZE_DWORD, REGSIZE_END);
#else // IS64BIT
static const RegSizeSet s_validImmSizeToRegMov(REGSIZE_BYTE, REGSIZE_WORD, REGSIZE_DWORD, REGSIZE_QWORD, REGSIZE_END);
#endif // IS64BIT

InstructionBase OpcodeMov::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  if(op2.getType() != IMMEDIATEVALUE) {
    return __super::operator()(op1,op2);
  } else {
    isValidOperandCombination(op1,op2,true);
    switch(op1.getType()) {
    case REGISTER     : return InstructionMovImm(m_regImmCode).setRegImm(op1.getRegister()  , op2.getImmInt64());
    case MEMORYOPERAND: return InstructionMovImm(m_memImmCode).setMemImm((MemoryOperand&)op1, op2.getImmInt32());
    }
  }
  throwInvalidOperandCombination(op1,op2);
  return __super::operator()(op1,op2); // should never come here
}

bool OpcodeMov::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError) const {
  if((op1.getType() == REGISTER) && (op2.getType() == IMMEDIATEVALUE)) {
    if(!validateRegisterOperand(op1, 1, throwOnError)) {
      return false;
    }
    return validateImmediateValue(op1.getSize(), op2, s_validImmSizeToRegMov, throwOnError);
  }
  return __super::isValidOperandCombination(op1,op2, throwOnError);
}
