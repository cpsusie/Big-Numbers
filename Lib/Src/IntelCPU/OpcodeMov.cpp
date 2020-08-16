#include "pch.h"

namespace IntelCPU {

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
  if(!op2.isImmediateValue()) {
    if(isGPR0ImmAddrPair(op1, op2)) {
      return m_GPR0tf.isValidOperandCombination(op1, op2, throwOnError);
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
  if(!op2.isImmediateValue()) {
    if(isGPR0ImmAddrPair(op1, op2)) {
      return m_GPR0tf(op2, op1);
    } else if(isOneSegmentRegister(op1, op2)) {
      return m_movSegCode(op1, op2);
    } else {
      return __super::operator()(op1, op2);
    }
  } else { // r/m, imm
    switch(op1.getType()) {
    case REGISTER     :
      if((op1.getSize() == REGSIZE_QWORD) && Register::sizeContainsSrcSize(REGSIZE_DWORD,op2.getSize())) {
        return m_memImmCode(op1,op2);
      }
      return m_regImmCode(op1,op2);
    case MEMORYOPERAND:
      return m_memImmCode(op1,op2);
    }
  }
  throwInvalidOperandCombination(op1, op2);
  return __super::operator()(op1, op2); // should never come here
}

}; // namespace
