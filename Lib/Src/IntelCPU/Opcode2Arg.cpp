#include "pch.h"

namespace IntelCPU {

InstructionBase Opcode2Arg::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  isValidOperandCombination(op1,op2,true);
  switch(op1.getType()) {
  case REGISTER       :
    switch(op2.getType()) {
    case REGISTER       : // reg <- reg
      return InstructionBuilder(*this).setRegRegOperands(op1.getRegister(),op2.getRegister());
    case MEMORYOPERAND  : // reg <- mem
      return InstructionBuilder(*this).setMemoryRegOperands((MemoryOperand&)op2, op1.getRegister()).setDirBit(REGISTER,MEMORYOPERAND);
    case IMMEDIATEVALUE : // reg <- imm
      return InstructionBuilder(*this).setRegisterOperand(op1.getRegister()).setImmediateOperand(op2,op1.getLimitedSize(REGSIZE_DWORD));
    }
    break;
  case MEMORYOPERAND    :
    switch(op2.getType()) {
    case REGISTER       : // mem <- reg
      return InstructionBuilder(*this).setMemoryRegOperands((MemoryOperand&)op1, op2.getRegister()).setDirBit(MEMORYOPERAND,REGISTER);
    case IMMEDIATEVALUE : // mem <- imm
      return InstructionBuilder(*this).setMemoryOperand((MemoryOperand&)op1).setImmediateOperand(op2,op1.getLimitedSize(REGSIZE_DWORD));
    }
  }
  throwInvalidOperandCombination(op1,op2);
  return __super::operator()(op1,op2);
}

InstructionBase Opcode2ArgI::operator()(const InstructionOperand &dst, const InstructionOperand &imm) const {
  isValidOperandCombination(dst,imm,true);
  return InstructionBuilder(*this).setOperandSize(dst.getSize()).setImmediateOperand(imm,dst.getLimitedSize(REGSIZE_DWORD));
}

InstructionBase Opcode2ArgMI8::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  isValidOperandCombination(op1,op2,true);
  return InstructionBuilder(*this).setMemOrRegOperand(op1).setImmediateOperand(op2,REGSIZE_BYTE);
}

InstructionBase Opcode2ArgMINB8::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  isValidOperandCombination(op1,op2,true);
  return InstructionBuilder(*this).setMemOrRegOperand(op1).setImmediateOperand(op2,REGSIZE_BYTE);
}

class InstructionBuilderO : public InstructionBuilder {
public:
  InstructionBuilderO(const OpcodeBase &opcode) : InstructionBuilder(opcode) {
  }
  InstructionBuilder &setRegisterOperand(const Register &dst);
};

InstructionBuilder &InstructionBuilderO::setRegisterOperand(const Register &reg) {
  const BYTE    regIndex = reg.getIndex();
  const RegSize regSize  = reg.getSize();
  switch(regSize) {
  case REGSIZE_BYTE :
    or(regIndex&7);
    SETREXUNIFORMREGISTER(reg);
    break;
  case REGSIZE_WORD :
    or(8 | (regIndex&7)).wordIns();
    break;
  case REGSIZE_DWORD :
    or(8 | (regIndex&7));
    break;
  case REGSIZE_QWORD :
    or(8 | (regIndex&7)).qwordIns();
    break;
  }
  SETREXBITS(HIGHINDEXTOREX(regIndex,REX_B));
  return *this;
}

InstructionBase Opcode2ArgOI::operator()(const InstructionOperand &dst, const InstructionOperand &imm) const {
  isValidOperandCombination(dst,imm,true);
  return InstructionBuilderO(*this).setRegisterOperand(dst.getRegister()).setImmediateOperand(imm,dst.getSize());
}

InstructionBase Opcode2ArgM::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  isValidOperandCombination(op1,op2,true);
  return InstructionBuilder(*this).setMemOrRegOperand(op2);
}

bool OpcodeStd2Arg::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError) const {
  if(!op2.isImmediateValue()) {
    return __super::isValidOperandCombination(op1,op2,throwOnError);
  } else if(op1.isGPR0()) {
    return m_codeI.isValidOperandCombination(op1,op2,throwOnError);
  } else {
    return m_codeMI.isValidOperandCombination(op1,op2,throwOnError);
  }
}

InstructionBase OpcodeStd2Arg::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  if(!op2.isImmediateValue()) {
    return __super::operator()(op1, op2);
  } else if(!(m_codeMI.getFlags() & HAS_IMM_XBIT)) {
    if(op1.isGPR0()) {
      return m_codeI(op1,op2);
    } else {
      return m_codeMI(op1,op2);
    }
  } else { // imm-value can be short if Byte-size
    if(op2.isImmByte() && !op1.isByte()) {
      return m_codeMI(op1,op2);
    } else if(op1.isGPR0()) {
      return m_codeI(op1,op2);
    } else {
      return m_codeMI(op1,op2);
    }
  }
}

bool Opcode2ArgDstGtSrc::isCompatibleSize(OperandSize dstSize, OperandSize srcSize) const {
  return Register::sizeBiggerThanSrcSize(dstSize, srcSize);
}

InstructionBase Opcode2ArgPfxF2::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  return InstructionBuilder(__super::operator()(op1,op2)).prefix(0xF2);
}

bool Opcode2ArgPfxF2SD::validateCompatibleSize(const Register &reg, const InstructionOperand &op, bool throwOnError) const {
  if(reg.isXMM() && op.isMemoryRef()) {
    if(op.getSize() == REGSIZE_MMWORD) {
      return true;
    }
  }
  return __super::validateCompatibleSize(reg,op,throwOnError);
}

}; // namespace
