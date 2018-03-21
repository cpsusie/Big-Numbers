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
      prefixImm(IMMOP,regSize,true ).setModeBits(MR_REGIMM(regIndex)).add((char)immv);
    }
    break;
  case REGSIZE_WORD :
    if(!isWord(immv)) sizeError(method,reg,immv);
    if(isByte(immv)) {
      prefixImm(IMMOP,regSize,true ).setModeBits(MR_REGIMM(regIndex)).add((char)immv);
    } else if(regIndex == 0) {
      or(0x04).add(immv,2).setOperandSize(regSize);
    } else {
      prefixImm(IMMOP,regSize,false).setModeBits(MR_REGIMM(regIndex)).add(immv,2);
    }
    break;
  default           :
    if(isByte(immv)) {
      prefixImm(IMMOP,regSize,true ).setModeBits(MR_REGIMM(regIndex)).add((char)immv);
    } else if(regIndex == 0) {
      or(0x04).add(immv,4).setOperandSize(regSize);
    } else {
      prefixImm(IMMOP,regSize,false).setModeBits(MR_REGIMM(regIndex)).add(immv,4);
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

const RegSizeSet Opcode2Arg::s_wordRegCapacity( REGSIZE_BYTE, REGSIZE_WORD, REGSIZE_END);
const RegSizeSet Opcode2Arg::s_dwordRegCapacity(REGSIZE_BYTE, REGSIZE_WORD, REGSIZE_DWORD, REGSIZE_END);
const RegSizeSet Opcode2Arg::s_qwordRegCapacity(REGSIZE_BYTE, REGSIZE_WORD, REGSIZE_DWORD, REGSIZE_QWORD, REGSIZE_END);

bool Opcode2Arg::sizeContainsSrcSize(OperandSize dstSize, OperandSize srcSize) { // static
  switch(dstSize) {
  case REGSIZE_BYTE  : return srcSize == REGSIZE_BYTE;
  case REGSIZE_WORD  : return s_wordRegCapacity.contains( srcSize);
  case REGSIZE_DWORD : return s_dwordRegCapacity.contains(srcSize);
  case REGSIZE_QWORD : return s_qwordRegCapacity.contains(srcSize);
  default            : return false;
  }
}

const RegSizeSet Opcode2Arg::s_validImmSizeToMem(   REGSIZE_BYTE, REGSIZE_WORD, REGSIZE_DWORD, REGSIZE_END);
const RegSizeSet Opcode2Arg::s_validImmSizeToReg(   REGSIZE_BYTE, REGSIZE_WORD, REGSIZE_DWORD, REGSIZE_END);

bool Opcode2Arg::isValidOperandCombination(const Register &reg, const InstructionOperand &op2) const {
  if(!isRegisterAllowed(reg)) return false;
  IF_NOT_REXCOMPATIBLE_RETURN_FALSE(reg,op2.needREXByte());
  switch(op2.getType()) {
  case REGISTER       :
    { const Register &regSrc = op2.getRegister();
      if(!isRegisterTypeAllowed(regSrc.getType())) return false;
      IF_NOT_REXCOMPATIBLE_RETURN_FALSE(regSrc,reg.indexNeedREXByte());
      return reg.getSize() == regSrc.getSize();
    }
  case MEMORYOPERAND  : // reg <- mem
    if(!isMemoryOperandAllowed((MemoryOperand&)op2)) return false;
    return reg.getSize() == op2.getSize();
  case IMMEDIATEVALUE :
    if(!isImmediateValueAllowed()) return false;
    return sizeContainsSrcSize(reg.getSize(), op2.getSize()) && s_validImmSizeToReg.contains(op2.getSize());
  }
  return false;
}

bool Opcode2Arg::isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2) const {
  switch(op1.getType()) {
  case REGISTER       :
    return isValidOperandCombination(op1.getRegister(), op2);
  case MEMORYOPERAND  :
    if(!isMemoryOperandAllowed((MemoryOperand&)op1)) return false;
    switch(op2.getType()) {
    case REGISTER       : // mem <- reg
      { const Register &regSrc = op2.getRegister();
        if(!isRegisterAllowed(regSrc)) return false;
        IF_NOT_REXCOMPATIBLE_RETURN_FALSE(regSrc,op1.needREXByte());
        return op1.getSize() == regSrc.getSize();
      }
      break;
    case IMMEDIATEVALUE : // mem <- imm
      if(!isImmediateValueAllowed()) return false;
      return sizeContainsSrcSize(op1.getSize(), op2.getSize()) && s_validImmSizeToMem.contains(op2.getSize());
    }
    break;
  }
  return false;
}

InstructionBase Opcode2Arg::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  Instruction2Arg result(*this);
  switch(op1.getType()) {
  case REGISTER       :
    { const Register &reg1 = (GPRegister&)op1.getRegister();
      validateRegisterAllowed(reg1);
      switch(op2.getType()) {
      case REGISTER       :
        validateRegisterAllowed(op2.getRegister());
        validateSameSize(op1,op2);
        return result.set2GPReg((GPRegister&)reg1,(GPRegister&)op2.getRegister());
      case MEMORYOPERAND  : // reg <- mem
        validateMemoryOperandAllowed((MemoryOperand&)op2);
        validateSameSize(op1,op2);
        return result.setGPRegMem((GPRegister&)reg1, (MemoryOperand&)op2);
      case IMMEDIATEVALUE :
        validateImmediateValueAllowed();
        VALIDATEISREXCOMPATIBLE(reg1, op2);
        return result.setGPRegImm((GPRegister&)reg1, op2.getImmInt32());
      }
    }
    break;
  case MEMORYOPERAND    :
    validateMemoryOperandAllowed((MemoryOperand&)op1);
    switch(op2.getType()) {
    case REGISTER       : // mem <- reg
      validateRegisterAllowed(op2.getRegister());
      validateSameSize(op1,op2);
      return result.setMemGPReg((MemoryOperand&)op1, (GPRegister&)op2.getRegister());
    case IMMEDIATEVALUE : // mem <- imm
      validateImmediateValueAllowed();
      return result.setMemImm((MemoryOperand&)op1, op2.getImmInt32());
    }
  }
  throwInvalidOperandCombination(__TFUNCTION__,op1,op2);
  return result;
}

void Opcode2Arg::throwInvalidOperandCombination(const TCHAR *method, const InstructionOperand &op1, const InstructionOperand &op2) { // static
  throwInvalidArgumentException(method
                               ,_T("Invalid combination of operands:%s,%s")
                               ,op1.toString().cstr()
                               ,op2.toString().cstr()
                               );
}
