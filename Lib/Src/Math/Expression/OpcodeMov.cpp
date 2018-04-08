#include "pch.h"
#include "InstructionBuilder.h"

class InstructionMovImm : public InstructionBuilder {
public:
  InstructionMovImm(const OpcodeBase &opcode) : InstructionBuilder(opcode) {
  }
  InstructionBuilder &setRegImm(const Register      &dst, const InstructionOperand &imm);
  inline InstructionBuilder &setMemImm(const MemoryOperand &dst, const InstructionOperand &imm) {
    return setMemoryOperand(dst).addImmediateOperand(imm,dst.getLimitedSize(REGSIZE_DWORD));
  }
};

InstructionBuilder &InstructionMovImm::setRegImm(const Register &reg, const InstructionOperand &imm) {
  DEFINEMETHODNAME;
  const BYTE    regIndex = reg.getIndex();
  const RegSize regSize  = reg.getSize();
  switch(regSize) {
  case REGSIZE_BYTE :
    or(regIndex&7).addImmediateOperand(imm,regSize);
    break;
  case REGSIZE_WORD :
    or(8 | (regIndex&7)).wordIns().addImmediateOperand(imm,regSize);
    break;
  case REGSIZE_DWORD :
    or(8 | (regIndex&7)).addImmediateOperand(imm,regSize);
    break;
  case REGSIZE_QWORD :
    if(Register::sizeContainsSrcSize(REGSIZE_DWORD,imm.getSize())) {
      xor(0x77).add(0xC0 | (regIndex&7)).addImmediateOperand(imm,REGSIZE_DWORD);
    } else {
      or(8 | (regIndex&7)).addImmediateOperand(imm,regSize);
    }
    qwordIns();
    break;
  }
  SETREXBITS(HIGHINDEXTOREX(regIndex,0));
  return *this;
}

static inline bool isGPR0AndImmAddr(const InstructionOperand &op1, const InstructionOperand &op2) {
  return op1.isGPR0() && op2.isDisplaceOnly();
}

static inline bool isGPR0ImmAddrPair(const InstructionOperand &op1, const InstructionOperand &op2) {
  return isGPR0AndImmAddr(op1, op2) || isGPR0AndImmAddr(op2, op1);
}

static inline bool isOneSegmentRegister(const InstructionOperand &op1, const InstructionOperand &op2) {
  return op1.isRegister(REGTYPE_SEG) != op2.isRegister(REGTYPE_SEG);
}

bool OpcodeMov::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError) const {
  if(op2.getType() != IMMEDIATEVALUE) {
    if(isGPR0ImmAddrPair(op1, op2)) {
      return m_GPR0AddrCode.isValidOperandCombination(op1, op2, throwOnError);
    } else if(isOneSegmentRegister(op1, op2)) {
      return m_movSegCode.isValidOperandCombination(op1, op2, throwOnError);
    }
  } else { // op2.type == IMMEDIATEVALUE
    switch(op1.getType()) {
    case REGISTER     : return m_regImmCode.isValidOperandCombination(op1, op2, throwOnError);
    case MEMORYOPERAND: return m_memImmCode.isValidOperandCombination(op1, op2, throwOnError);
    }
  }
  return __super::isValidOperandCombination(op1, op2, throwOnError);
}

InstructionBase OpcodeMov::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  if(op2.getType() != IMMEDIATEVALUE) {
    if(isGPR0ImmAddrPair(op1, op2)) {
      return m_GPR0AddrCode(op2, op1);
    } else if(isOneSegmentRegister(op1, op2)) {
      return m_movSegCode(op1, op2);
    } else {
      return __super::operator()(op1, op2);
    }
  } else {
    switch(op1.getType()) {
    case REGISTER     :
      m_regImmCode.isValidOperandCombination(op1, op2, true);
      return InstructionMovImm(m_regImmCode).setRegImm(op1.getRegister()  , op2);
    case MEMORYOPERAND:
      m_memImmCode.isValidOperandCombination(op1, op2, true);
      return InstructionMovImm(m_memImmCode).setMemImm((MemoryOperand&)op1, op2);
    }
  }
  throwInvalidOperandCombination(op1, op2);
  return __super::operator()(op1, op2); // should never come here
}
