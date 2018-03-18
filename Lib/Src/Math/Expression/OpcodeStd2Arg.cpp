#include "pch.h"
#include <Math/Expression/NewOpCode.h>
#include "RexByte.h"

static void sizeError(const TCHAR *method, const GPRegister    &reg, INT64 immv) {
  throwInvalidArgumentException(method,_T("Immediate value %s doesn't fit in %s"),formatHexValue(immv,false).cstr(), reg.getName().cstr());
}
static void sizeError(const TCHAR *method, const MemoryOperand &memop, INT64 immv) {
  throwInvalidArgumentException(method,_T("Immediate value %s doesn't fit in %s"),formatHexValue(immv,false).cstr(), memop.toString().cstr());
}

class InstructionStd2Arg : public InstructionBuilder {
public:
  InstructionStd2Arg(const OpcodeBase &opcode) : InstructionBuilder(opcode)
  {
  }
  // Assume sáme size of dst and src
  InstructionStd2Arg &set2GPReg(  const GPRegister    &dst, const GPRegister    &src);
  InstructionStd2Arg &setGPRegMem(const GPRegister    &dst, const MemoryOperand &src);
  InstructionStd2Arg &setGPRegImm(const GPRegister    &dst, int   immv              );
  InstructionStd2Arg &setMemGPReg(const MemoryOperand &dst, const GPRegister    &src);
  InstructionStd2Arg &setMemImm(  const MemoryOperand &dst, int   immv              );
};

InstructionStd2Arg &InstructionStd2Arg::set2GPReg(const GPRegister &dst, const GPRegister &src) {
  const BYTE    srcIndex = src.getIndex();
  const BYTE    dstIndex = dst.getIndex();
  const RegSize size     = src.getSize();
  switch(size) {
  case REGSIZE_BYTE :
    or(2).add(0xC0 | ((dstIndex&7)<<3) | (srcIndex&7));
    break;
  case REGSIZE_WORD : wordIns();
    // continue case
  default           :
    or(3).add(0xC0 | ((dstIndex&7)<<3) | (srcIndex&7));
    break;
  }
  SETREXBITS(QWORDTOREX(size)|HIGHINDEXTOREX(dstIndex,2)|HIGHINDEXTOREX(srcIndex,0))
  return *this;
}

InstructionStd2Arg &InstructionStd2Arg::setGPRegMem(const GPRegister    &dst, const MemoryOperand &src) {
  or(2);
  return setMemGPReg(src, dst);
}

InstructionStd2Arg &InstructionStd2Arg::setGPRegImm(const GPRegister &reg, int immv) {
  DEFINEMETHODNAME;
  const BYTE    regIndex = reg.getIndex();
  const RegSize regSize  = reg.getSize();
  switch(regSize) {
  case REGSIZE_BYTE :
    if(!isByte(immv)) sizeError(method,reg,immv);
    if(regIndex == 0) {
      or(0x04).add((char)immv);
    } else {
      prefix(0x80).or(0xC0 | (regIndex&7)).add((char)immv);
    }
    break;
  case REGSIZE_WORD :
    if(!isWord(immv)) sizeError(method,reg,immv);
    if(isByte(immv)) {
      prefix(0x83).or(0xC0 | (regIndex&7)).add((char)immv);
    } else if(regIndex == 0) {
      or(0x05).add(immv,2);
    } else {
      prefix(0x81).or(0xC0 | (regIndex&7)).add(immv,2);
    }
    wordIns();
    break;
  default           :
    if(isByte(immv)) {
      prefix(0x83).or(0xC0 | (regIndex&7)).add((char)immv);
    } else if(regIndex == 0) {
      or(0x05).add(immv,4);
    } else {
      prefix(0x81).or(0xC0 | (regIndex&7)).add(immv,4);
    }
    break;
  }
  SETREXBITS(QWORDTOREX(regSize)|HIGHINDEXTOREX(regIndex,0));
  return *this;
}

InstructionStd2Arg &InstructionStd2Arg::setMemGPReg(const MemoryOperand &dst, const GPRegister &src) {
  const BYTE    regIndex = src.getIndex();
  const RegSize regSize  = src.getSize();
  switch(regSize) {
  case REGSIZE_BYTE :
    addMemoryReference(dst);
    or(getArgIndex(), (regIndex&7)<<3);
    break;
  case REGSIZE_WORD :
    wordIns();
    // continue case
  default           :
    or(1).addMemoryReference(dst);
    or(getArgIndex(), (regIndex&7)<<3);
    break;
  }
  SETREXBITS(QWORDTOREX(regSize)|HIGHINDEXTOREX(regIndex,2));
  return *this;
}

InstructionStd2Arg &InstructionStd2Arg::setMemImm(const MemoryOperand &dst, int immv) {
  DEFINEMETHODNAME;
  const OperandSize size = dst.getSize();
  switch(size) {
  case REGSIZE_BYTE :
    if(!isByte(immv)) sizeError(method,dst,immv);
    prefix(0x80).setMemoryReference(dst).add((char)immv);
    break;
  case REGSIZE_WORD :
    if(!isWord(immv)) sizeError(method,dst,immv);
    prefix(isByte(immv)?0x83:0x81).setMemoryReference(dst).add(immv,isByte(immv)?1:2);
    wordIns();
    break;
  default           :
    prefix(isByte(immv)?0x83:0x81).setMemoryReference(dst).add(immv,isByte(immv)?1:4);
    break;
  }
  SETREXBITS(QWORDTOREX(size));
  return *this;
}

static const RegSizeSet s_wordRegCapacity( REGSIZE_BYTE, REGSIZE_WORD, REGSIZE_END);
static const RegSizeSet s_dwordRegCapacity(REGSIZE_BYTE, REGSIZE_WORD, REGSIZE_DWORD, REGSIZE_END);
static const RegSizeSet s_qwordRegCapacity(REGSIZE_BYTE, REGSIZE_WORD, REGSIZE_DWORD, REGSIZE_QWORD, REGSIZE_END);

static bool sizeContainsSrcSize(OperandSize dstSize, OperandSize srcSize) {
  switch(dstSize) {
  case REGSIZE_BYTE  : return srcSize == REGSIZE_BYTE;
  case REGSIZE_WORD  : return s_wordRegCapacity.contains( srcSize);
  case REGSIZE_DWORD : return s_dwordRegCapacity.contains(srcSize);
  case REGSIZE_QWORD : return s_qwordRegCapacity.contains(srcSize);
  default            : return false;
  }
}

static const RegSizeSet s_validImmSizeToMem(   REGSIZE_BYTE, REGSIZE_WORD, REGSIZE_DWORD, REGSIZE_END);
static const RegSizeSet s_validImmSizeToReg(   REGSIZE_BYTE, REGSIZE_WORD, REGSIZE_DWORD, REGSIZE_END);
#ifdef IS32BIT
#define s_validImmSizeToRegMov s_validImmSizeToReg
#else // IS64BIT
static const RegSizeSet s_validImmSizeToRegMov(REGSIZE_BYTE, REGSIZE_WORD, REGSIZE_DWORD, REGSIZE_QWORD, REGSIZE_END);
#endif // IS64BIT

bool Opcode2Arg::isValidOperandCombination(const Register &reg, const InstructionOperand &op2) const {
  if(!isRegisterAllowed(reg)) return false;
#ifdef IS64BIT
    if(!reg.isREXCompatible(op2.needREXByte())) return false;
#endif // IS64BIT

  switch(op2.getType()) {
  case REGISTER       :
    { const Register &regSrc = op2.getRegister();
      if(!isRegisterTypeAllowed(regSrc.getType())) return false;
#ifdef IS64BIT
      if(!regSrc.isREXCompatible(reg.indexNeedREXByte())) return false;
#endif // IS64BIT
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
#ifdef IS64BIT
        if(!regSrc.isREXCompatible(op1.needREXByte())) return false;
#endif // IS64BIT
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
  InstructionStd2Arg result(*this);
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
#ifdef IS64BIT
        validateIsRexCompatible(reg1, op2);
#endif // IS64BIT
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
  throwInvalidArgumentException(__TFUNCTION__,_T("Invalid combination of operands:%s,%s")
                               ,op1.toString().cstr()
                               ,op2.toString().cstr()
                               );
  return result;
}

class InstructionMovImm : public InstructionBuilder {
public:
  InstructionMovImm(const OpcodeBase &opcode) : InstructionBuilder(opcode)
  {
  }
  InstructionMovImm &setGPRegImm(const GPRegister    &dst, INT64 immv);
  InstructionMovImm &setMemImm(  const MemoryOperand &dst, int   immv);
};

InstructionMovImm &InstructionMovImm::setGPRegImm(const GPRegister &reg, INT64 immv) {
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
    or(0x08 | (regIndex&7)).add(immv,2).wordIns();
    break;
  case REGSIZE_DWORD :
    if(!isDword(immv)) sizeError(method,reg,immv);
    or(0x08 | (regIndex&7)).add(immv,4);
    break;
  case REGSIZE_QWORD :
    if(isDword(immv)) {
      xor(0x77).add(0xC0 | (regIndex&7)).add(immv,4);
    } else {
      or(0x08 | (regIndex&7)).add(immv,8);
    }
    break;
  }
  SETREXBITS(QWORDTOREX(regSize)|HIGHINDEXTOREX(regIndex,0));
  return *this;
}

InstructionMovImm &InstructionMovImm::setMemImm(const MemoryOperand &dst, int immv) {
  DEFINEMETHODNAME;
  const OperandSize size = dst.getSize();
  switch(size) {
  case REGSIZE_BYTE :
    if(!isByte(immv)) sizeError(method,dst,immv);
    xor(0x76).addMemoryReference(dst).add((char)immv);
    break;
  case REGSIZE_WORD :
    if(!isWord(immv)) sizeError(method,dst,immv);
    xor(0x77).addMemoryReference(dst).add(immv,2).wordIns();
    break;
  default:
    xor(0x77).addMemoryReference(dst).add(immv,4);
    break;
  }
  SETREXBITS(QWORDTOREX(size));
  return *this;
}

InstructionBase OpcodeMovImm::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  assert(op2.getType() == IMMEDIATEVALUE);
  InstructionMovImm result(*this);
  switch(op1.getType()) {
  case REGISTER       :
    { const Register &reg = op1.getRegister();
      validateRegisterAllowed(reg);
#ifdef IS64BIT
      validateIsRexCompatible(reg,op2);
#endif // IS64BIT
      return result.setGPRegImm((GPRegister&)reg, op2.getImmInt64());
    }
  case MEMORYOPERAND    :
    validateMemoryOperandAllowed((MemoryOperand&)op1);
    return result.setMemImm((MemoryOperand&)op1, op2.getImmInt32());
  default:
    throwInvalidArgumentException(__TFUNCTION__,_T("Invalid combination of operands:%s,%s")
                                  ,op1.toString().cstr()
                                  ,op2.toString().cstr()
                                  );
    return result;
  }
}

bool OpcodeMovImm::isValidOperandCombination(const Register &reg, const InstructionOperand &op2) const {
  if(op2.getType() != IMMEDIATEVALUE) {
    return __super::isValidOperandCombination(reg, op2);
  } else {
    if(!isRegisterAllowed(reg)) return false;
    if(!isImmediateValueAllowed()) return false;
#ifdef IS64BIT
    if(!reg.isREXCompatible(false)) return false;
#endif // IS64BIT
    return sizeContainsSrcSize(reg.getSize(), op2.getSize()) && s_validImmSizeToRegMov.contains(op2.getSize());
  }
}

InstructionBase OpcodeMov::operator()(const InstructionOperand &op1, const InstructionOperand &op2) const {
  if(op2.getType() != IMMEDIATEVALUE) {
    return __super::operator()(op1,op2);
  } else {
    return m_immCode(op1, op2);
  }
}

bool OpcodeMov::isValidOperandCombination(const Register &reg, const InstructionOperand &op2) const {
  return m_immCode.isValidOperandCombination(reg,op2);
}
