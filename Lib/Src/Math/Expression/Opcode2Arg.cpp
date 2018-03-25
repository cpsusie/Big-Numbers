#include "pch.h"
#include "InstructionBuilder.h"

class Instruction2Arg : public InstructionBuilder {
public:
  Instruction2Arg(const OpcodeBase &opcode) : InstructionBuilder(opcode)
  {
  }
  // Assume same size of dst and src
  InstructionBuilder &set2GPReg(  const GPRegister    &dst, const GPRegister    &src) {
    return setRegRegOperands(dst, src);
  }
  InstructionBuilder &setGPRegMem(const GPRegister    &dst, const MemoryOperand &src) {
    return setMemGPReg(src, dst).setDirectionBit();
  }
  InstructionBuilder &setGPRegImm(const GPRegister    &dst, int   immv              );
  InstructionBuilder &setMemGPReg(const MemoryOperand &dst, const GPRegister    &src) {
    return setMemoryRegOperands(dst, src);
  }
  InstructionBuilder &setMemImm(  const MemoryOperand &dst, int   immv              );
};

#define IMMOP 0x80
InstructionBuilder &Instruction2Arg::setGPRegImm(const GPRegister &reg, int immv) {
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

InstructionBuilder &Instruction2Arg::setMemImm(const MemoryOperand &dst, int immv) {
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
  Instruction2Arg result(*this);
  switch(op1.getType()) {
  case REGISTER       :
    { const GPRegister &reg1 = (GPRegister&)op1.getRegister();
      switch(op2.getType()) {
      case REGISTER       :
        return result.set2GPReg(reg1,(GPRegister&)op2.getRegister());
      case MEMORYOPERAND  : // reg <- mem
        return result.setGPRegMem(reg1, (MemoryOperand&)op2);
      case IMMEDIATEVALUE :
        return result.setGPRegImm(reg1, op2.getImmInt32());
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
  throwInvalidOperandCombination(op1,op2);
  return result;
}
