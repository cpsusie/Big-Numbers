#include "pch.h"
#include "InstructionBuilder.h"

class Instruction2ArgImm : public InstructionBuilder {
public:
  Instruction2ArgImm(const OpcodeBase &opcode) : InstructionBuilder(opcode) {
  }
  InstructionBuilder &setRegImm(const Register      &dst, int   immv);
  InstructionBuilder &setMemImm(const MemoryOperand &dst, int   immv);
};

#define IMMOP 0x80
InstructionBuilder &Instruction2ArgImm::setRegImm(const Register &reg, int immv) {
  DEFINEMETHODNAME;
  const BYTE    regIndex  = reg.getIndex();
  const RegSize regSize   = reg.getSize();
  const bool    immIsByte = isByte(immv);
  switch(regSize) {
  case REGSIZE_BYTE :
    if(!immIsByte) sizeError(method,reg,immv);
    if(regIndex == 0) {
      or(0x04).add((char)immv).setOperandSize(regSize);
    } else {
      prefixImm(IMMOP,regSize,true ).setModeBits(MR_REG(regIndex)).add((char)immv);
    }
    break;
  case REGSIZE_WORD :
    if(!isWord(immv)) sizeError(method,reg,immv);
    if(immIsByte) {
      prefixImm(IMMOP,regSize,true ).setModeBits(MR_REG(regIndex)).add((char)immv);
    } else if(regIndex == 0) {
      or(0x04).add((BYTE*)&immv,2).setOperandSize(regSize);
    } else {
      prefixImm(IMMOP,regSize,false).setModeBits(MR_REG(regIndex)).add((BYTE*)&immv,2);
    }
    break;
  default           :
    if(immIsByte) {
      prefixImm(IMMOP,regSize,true ).setModeBits(MR_REG(regIndex)).add((char)immv);
    } else if(regIndex == 0) {
      or(0x04).add((BYTE*)&immv,4).setOperandSize(regSize);
    } else {
      prefixImm(IMMOP,regSize,false).setModeBits(MR_REG(regIndex)).add((BYTE*)&immv,4);
    }
    break;
  }
  SETREXBITS(HIGHINDEXTOREX(regIndex,0));
  return *this;
}

InstructionBuilder &Instruction2ArgImm::setMemImm(const MemoryOperand &dst, int immv) {
  DEFINEMETHODNAME;
  const OperandSize size      = dst.getSize();
  const bool        immIsByte = isByte(immv);
  switch(size) {
  case REGSIZE_BYTE :
    if(!immIsByte) sizeError(method,dst,immv);
    prefixImm(IMMOP,size,true).setMemoryOperand(dst).add((char)immv);
    break;
  case REGSIZE_WORD :
    if(!isWord(immv)) sizeError(method,dst,immv);
    prefixImm(IMMOP,size,immIsByte).setMemoryOperand(dst).add((BYTE*)&immv,immIsByte?1:2);
    break;
  default           :
    prefixImm(IMMOP,size,immIsByte).setMemoryOperand(dst).add((BYTE*)&immv,immIsByte?1:4);
    break;
  }
  return *this;
}

InstructionBase Opcode2Arg::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  isValidOperandCombination(op1,op2,true);
  switch(op1.getType()) {
  case REGISTER       :
    switch(op2.getType()) {
    case REGISTER       : // reg <- reg
      return InstructionBuilder(*this).setRegRegOperands(op1.getRegister(),op2.getRegister());
    case MEMORYOPERAND  : // reg <- mem
      return InstructionBuilder(*this).setMemoryRegOperands((MemoryOperand&)op2, op1.getRegister()).setDirectionBit();
    case IMMEDIATEVALUE : // reg <- imm
      return Instruction2ArgImm(*this).setRegImm(op1.getRegister(), op2.getImmInt32());
    }
    break;
  case MEMORYOPERAND    :
    switch(op2.getType()) {
    case REGISTER       : // mem <- reg
      return InstructionBuilder(*this).setMemoryRegOperands((MemoryOperand&)op1, op2.getRegister());
    case IMMEDIATEVALUE : // mem <- imm
      return Instruction2ArgImm(*this).setMemImm((MemoryOperand&)op1, op2.getImmInt32());
    }
  }
  throwInvalidOperandCombination(op1,op2);
  return __super::operator()(op1,op2);
}
