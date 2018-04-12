#include "pch.h"
#include "InstructionBuilder.h"

InstructionBase Opcode2Arg::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  isValidOperandCombination(op1,op2,true);
  switch(op1.getType()) {
  case REGISTER       :
    switch(op2.getType()) {
    case REGISTER       : // reg <- reg
      return InstructionBuilder(*this).setRegRegOperands(op1.getRegister(),op2.getRegister());
    case MEMORYOPERAND  : // reg <- mem
      return InstructionBuilder(*this).setMemoryRegOperands((MemoryOperand&)op2, op1.getRegister()).setDirectionBit(REGISTER,MEMORYOPERAND);
    }
    break;
  case MEMORYOPERAND    :
    switch(op2.getType()) {
    case REGISTER       : // mem <- reg
      return InstructionBuilder(*this).setMemoryRegOperands((MemoryOperand&)op1, op2.getRegister()).setDirectionBit(MEMORYOPERAND,REGISTER);
    }
  }
  throwInvalidOperandCombination(op1,op2);
  return __super::operator()(op1,op2);
}

InstructionBase Opcode2ArgI::operator()(const InstructionOperand &dst, const InstructionOperand &imm) const {
  isValidOperandCombination(dst,imm,true);
  return InstructionBuilder(*this).setOperandSize(dst.getSize()).addImmediateOperand(imm,dst.getLimitedSize(REGSIZE_DWORD));
}

InstructionBase Opcode2ArgMI::operator()(const InstructionOperand &dst, const InstructionOperand &imm) const {
  isValidOperandCombination(dst,imm,true);
  if((getFlags() & HAS_IMM_XBIT) && imm.isImmByte()) {
    return InstructionBuilder(*this).setMemOrRegOperand(dst).setImmediateOperand(imm,&dst);
  } else {
    return InstructionBuilder(*this).setMemOrRegOperand(dst).addImmediateOperand(imm,dst.getLimitedSize(REGSIZE_DWORD));
  }
}

InstructionBase Opcode2ArgM::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  isValidOperandCombination(op1,op2,true);
  return InstructionBuilder(*this).setMemOrRegOperand(op2);
}

bool OpcodeStd2Arg::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError) const {
  if(op2.getType() != IMMEDIATEVALUE) {
    return __super::isValidOperandCombination(op1,op2,throwOnError);
  } else if(op1.isGPR0()) {
    return m_codeI.isValidOperandCombination(op1,op2,throwOnError);
  } else {
    return m_codeMI.isValidOperandCombination(op1,op2,throwOnError);
  }
}

InstructionBase OpcodeStd2Arg::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  if(op2.getType() != IMMEDIATEVALUE) {
    return __super::operator()(op1, op2);
  } else {
    if(!(m_codeMI.getFlags() & HAS_IMM_XBIT)) {
      if(op1.isGPR0()) {
        return m_codeI(op1,op2);
      } else {
        return m_codeMI(op1,op2);
      }
    } else { // imm-value can be short if Byte-size
      if(op2.isImmByte() && (op1.getSize() != REGSIZE_BYTE)) {
        return m_codeMI(op1,op2);
      } else if(op1.isGPR0()) {
        return m_codeI(op1,op2);
      } else {
        return m_codeMI(op1,op2);
      }
    }
  }
}

InstructionBase Opcode2ArgPfxF2::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  return InstructionBuilder(__super::operator()(op1,op2)).prefix(0xF2);
}

bool Opcode2ArgPfxF2SD::validateSameSize(const Register &reg, const InstructionOperand &op, bool throwOnError) const {
  if((reg.getType() == REGTYPE_XMM) && (op.getType() == MEMORYOPERAND)) {
    if(op.getSize() == REGSIZE_MMWORD) {
      return true;
    }
  }
  return __super::validateSameSize(reg,op,throwOnError);
}
