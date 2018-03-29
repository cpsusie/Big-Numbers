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

static inline bool isGPR0AndImmAddr(const InstructionOperand &op1, const InstructionOperand &op2) {
  return (op1.getType() == REGISTER)
      &&  op1.getRegister().isGPR0()
      && (op2.getType() == MEMORYOPERAND)
      &&  op2.getMemoryReference().isDisplaceOnly();
}

static inline bool isGPR0ImmAddrPair(const InstructionOperand &op1, const InstructionOperand &op2) {
  return isGPR0AndImmAddr(op1,op2) || isGPR0AndImmAddr(op2,op1);
}

InstructionBase OpcodeMov::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  if(op2.getType() != IMMEDIATEVALUE) {
    if(isGPR0ImmAddrPair(op1, op2)) {
      return m_GPR0AddrCode(op2,op1);
    }
    return __super::operator()(op1,op2);
  } else {
    switch(op1.getType()) {
    case REGISTER     :
      m_regImmCode.isValidOperandCombination(op1,op2,true);
      return InstructionMovImm(m_regImmCode).setRegImm(op1.getRegister()  , op2.getImmInt64());
    case MEMORYOPERAND:
      m_memImmCode.isValidOperandCombination(op1,op2,true);
      return InstructionMovImm(m_memImmCode).setMemImm((MemoryOperand&)op1, op2.getImmInt32());
    }
  }
  throwInvalidOperandCombination(op1,op2);
  return __super::operator()(op1,op2); // should never come here
}

bool OpcodeMov::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError) const {
  if(op2.getType() != IMMEDIATEVALUE) {
    if(isGPR0ImmAddrPair(op1, op2)) {
      return m_GPR0AddrCode.isValidOperandCombination(op1, op2, throwOnError);
    }
  } else { // op2.type == IMMEDIATEVALUE
    switch(op1.getType()) {
    case REGISTER     : return m_regImmCode.isValidOperandCombination(op1,op2,throwOnError);
    case MEMORYOPERAND: return m_memImmCode.isValidOperandCombination(op1,op2,throwOnError);
    }
  }
  return __super::isValidOperandCombination(op1,op2, throwOnError);
}
