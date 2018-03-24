#include "pch.h"
#include "InstructionBuilder.h"

class Instruction2Arg : public InstructionBuilder {
public:
  Instruction2Arg(const OpcodeBase &opcode) : InstructionBuilder(opcode)
  {
  }
  // Assume sáme size of dst and src
  InstructionBuilder &set2GPReg(  const GPRegister    &dst, const GPRegister    &src);
  InstructionBuilder &setGPRegMem(const GPRegister    &dst, const MemoryOperand &src);
  InstructionBuilder &setGPRegImm(const GPRegister    &dst, int   immv              );
  InstructionBuilder &setMemGPReg(const MemoryOperand &dst, const GPRegister    &src);
  InstructionBuilder &setMemImm(  const MemoryOperand &dst, int   immv              );
};

InstructionBuilder &Instruction2Arg::set2GPReg(const GPRegister &dst, const GPRegister &src) {
  const BYTE    srcIndex = src.getIndex();
  const BYTE    dstIndex = dst.getIndex();
  const RegSize size     = src.getSize();
  setDirectionBit().setOperandSize(size).setModeBits(MR_REGREG(dstIndex,srcIndex)); // set direction bit
  SETREXBITS(HIGHINDEXTOREX(dstIndex,2) | HIGHINDEXTOREX(srcIndex,0))
  return *this;
}

InstructionBuilder &Instruction2Arg::setGPRegMem(const GPRegister &dst, const MemoryOperand &src) {
  return setMemGPReg(src, dst).setDirectionBit();
}

#define IMMOP 0x80
InstructionBuilder &Instruction2Arg::setGPRegImm(const GPRegister &reg, int immv) {
  DEFINEMETHODNAME;
  const BYTE    regIndex = reg.getIndex();
  const RegSize regSize  = reg.getSize();
  switch(regSize) {
  case REGSIZE_BYTE :
    if(!isByte(immv)) sizeError(method,reg,immv);
    if(regIndex == 0) {
      or(0x04).add((char)immv).setOperandSize(regSize);
    } else {
      prefixImm(IMMOP,regSize,true ).setModeBits(MR_REG(regIndex)).add((char)immv);
    }
    break;
  case REGSIZE_WORD :
    if(!isWord(immv)) sizeError(method,reg,immv);
    if(isByte(immv)) {
      prefixImm(IMMOP,regSize,true ).setModeBits(MR_REG(regIndex)).add((char)immv);
    } else if(regIndex == 0) {
      or(0x04).add(immv,2).setOperandSize(regSize);
    } else {
      prefixImm(IMMOP,regSize,false).setModeBits(MR_REG(regIndex)).add(immv,2);
    }
    break;
  default           :
    if(isByte(immv)) {
      prefixImm(IMMOP,regSize,true ).setModeBits(MR_REG(regIndex)).add((char)immv);
    } else if(regIndex == 0) {
      or(0x04).add(immv,4).setOperandSize(regSize);
    } else {
      prefixImm(IMMOP,regSize,false).setModeBits(MR_REG(regIndex)).add(immv,4);
    }
    break;
  }
  SETREXBITS(HIGHINDEXTOREX(regIndex,0));
  return *this;
}

InstructionBuilder &Instruction2Arg::setMemGPReg(const MemoryOperand &dst, const GPRegister &src) {
  const BYTE    regIndex = src.getIndex();
  const RegSize regSize  = src.getSize();
  setMemoryOperand(dst).setModeBits((regIndex&7)<<3);
  SETREXBITS(HIGHINDEXTOREX(regIndex,2));
  return *this;
}

InstructionBuilder &Instruction2Arg::setMemImm(const MemoryOperand &dst, int immv) {
  DEFINEMETHODNAME;
  const OperandSize size = dst.getSize();
  switch(size) {
  case REGSIZE_BYTE :
    if(!isByte(immv)) sizeError(method,dst,immv);
    prefixImm(IMMOP,size,true).setMemoryOperand(dst).add((char)immv);
    break;
  case REGSIZE_WORD :
    if(!isWord(immv)) sizeError(method,dst,immv);
    prefixImm(IMMOP,size,isByte(immv)).setMemoryOperand(dst).add(immv,isByte(immv)?1:2);
    break;
  default           :
    prefixImm(IMMOP,size,isByte(immv)).setMemoryOperand(dst).add(immv,isByte(immv)?1:4);
    break;
  }
  return *this;
}

InstructionBase Opcode2Arg::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  isValidOperandCombination(op1,op2,true);
  Instruction2Arg result(*this);
  switch(op1.getType()) {
  case REGISTER       :
    { const Register &reg1 = (GPRegister&)op1.getRegister();
      switch(op2.getType()) {
      case REGISTER       :
        return result.set2GPReg((GPRegister&)reg1,(GPRegister&)op2.getRegister());
      case MEMORYOPERAND  : // reg <- mem
        return result.setGPRegMem((GPRegister&)reg1, (MemoryOperand&)op2);
      case IMMEDIATEVALUE :
        return result.setGPRegImm((GPRegister&)reg1, op2.getImmInt32());
      }
    }
    break;
  case MEMORYOPERAND    :
    switch(op2.getType()) {
    case REGISTER       : // mem <- reg
      return result.setMemGPReg((MemoryOperand&)op1, (GPRegister&)op2.getRegister());
    case IMMEDIATEVALUE : // mem <- imm
      return result.setMemImm((MemoryOperand&)op1, op2.getImmInt32());
    }
  }
  throwInvalidOperandCombination(__TFUNCTION__,op1,op2);
  return result;
}
