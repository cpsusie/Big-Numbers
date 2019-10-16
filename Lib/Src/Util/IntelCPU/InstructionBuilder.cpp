#include "pch.h"
#include "InstructionBuilder.h"

const RegSizeSet InstructionBuilder::s_sizeBitSet(REGSIZE_WORD, REGSIZE_DWORD, REGSIZE_QWORD, -1);

static inline BYTE findDirectionMask(const OpcodeBase &opcode) {
  const UINT flags = opcode.getFlags();
  if(flags & HAS_DIRBIT1) return 2;
  if(flags & HAS_DIRBIT0) return 1;
  return 0;
}

InstructionBuilder::InstructionBuilder(const OpcodeBase &opcode)
  : InstructionBase(opcode               )
  , m_flags(        opcode.getFlags()    )
  , m_extension(    opcode.getExtension())
  , m_opcodeSize(   opcode.size()        )
  , m_directionMask(findDirectionMask(opcode))
#ifdef _DEBUG
  , m_debugStr(opcode.getMnemonic())
#endif
{
  init();
  if(m_extension) {
    addExtension();
  }
}

InstructionBuilder::InstructionBuilder(const InstructionBase &ins, UINT flags)
  : InstructionBase(ins       )
  , m_flags(        flags     )
  , m_extension(    0         )
  , m_opcodeSize(   ins.size())
  , m_directionMask(0         )
#ifdef _DEBUG
  , m_debugStr(ins.toString())
#endif
{
  init();
}

InstructionBuilder &InstructionBuilder::insert(BYTE index, BYTE b) {
  assert((m_size < MAX_INSTRUCTIONSIZE) && (index<m_size));
  BYTE *bp = m_bytes + index;
  for(BYTE *dst = m_bytes + m_size++, *src = dst-1; dst > bp;) {
    *(dst--) = *(src--);
  }
  *bp = b;
  if(index <= m_opcodePos) m_opcodePos++;
  return *this;
}

#ifdef IS64BIT
InstructionBuilder &InstructionBuilder::setRexBits(BYTE bits) {
  assert((bits&0xf0)==0);
  if(m_hasRexByte) {
    m_bytes[m_rexByteIndex] |= bits; // just add new bits to rex-byte
    return *this;
  }
  m_hasRexByte = true;
  return insert(m_rexByteIndex, 0x40|bits);
}
#endif // IS64BIT

InstructionBuilder &InstructionBuilder::add(const void *src, BYTE count) {
  assert(m_size+count <= MAX_INSTRUCTIONSIZE);
  if(count == 1) {
    m_bytes[m_size++] = *(BYTE*)src;
  } else {
    for(BYTE *p = (BYTE*)src, *dst = m_bytes+m_size, *end = dst+count; dst<end;) {
      *(dst++) = *(p++);
    }
    m_size += count;
  }
  return *this;
}

InstructionBuilder &InstructionBuilder::setOperandSize(OperandSize size) {
  switch(size) {
  case REGSIZE_WORD :
    setSizeBit().wordIns();
    break;
  case REGSIZE_DWORD:
    setSizeBit();
    break;
  case REGSIZE_QWORD:
    setSizeBit().qwordIns();
    break;
  }
  return *this;
}

InstructionBuilder &InstructionBuilder::setModeBits(BYTE bits) {
  if(m_modeByteCreated) {
    if(bits) {
      or(getModeByteIndex(), bits);
    }
  } else {
    m_modeByteCreated = true;
    add(bits);
  }
  return *this;
}

// ptr[(inx<<shift)+offset], (inx&7) != 4, shift<=3, offset=[INT_MIN;INT_MAX]
InstructionBuilder &InstructionBuilder::addrShiftInx(const IndexRegister &inx, BYTE shift, int offset) {
  const BYTE inxIndex = inx.getIndex();
  assert(((inxIndex&7)!=4) && (shift<=3));
  SETREXBITONHIGHINX(inxIndex,REX_X);
  return setModeBits(MR_SIB(0)).add(SIB_BYTE(5,inxIndex,shift)).add(&offset, 4);
}

// ------------------------------------------------------------------------------------------

InstructionBuilder &InstructionBuilder::addrBase(const IndexRegister &base, int offset) {
  const BYTE baseIndex = base.getIndex();
  switch(baseIndex&7) {
  case 4 :
    if(offset == 0) {
      setModeBits(MR_SIB(DP_0BYTE)).add(0x24);                        // ptr[esp]
    } else if(isByte(offset)) {
      setModeBits(MR_SIB(DP_1BYTE)).add(0x24).add((char)offset);      // ptr[esp+1 byte offset]
    } else {
      setModeBits(MR_SIB(DP_4BYTE)).add(0x24).add(&offset, 4);        // ptr[esp+4 byte offset]
    }
    break;
  default:
    if((offset == 0) && ((baseIndex&7)!=5)) {
      setModeBits(baseIndex&7);                                       // ptr[base],               (base&7) != {4,5}
    } else if(isByte(offset)) {
      setModeBits(MR_DP1BYTE(baseIndex)).add((char)offset);           // ptr[base+1 byte offset], (base&7) != 4
    } else {
      setModeBits(MR_DP4BYTE(baseIndex)).add(&offset, 4);             // ptr[base+4 byte offset], (base&7) != 4
    }
  }
  SETREXBITONHIGHINX(baseIndex,REX_B);
  return *this;
}

InstructionBuilder &InstructionBuilder::addrBaseShiftInx(const IndexRegister &base, const IndexRegister &inx, BYTE shift, int offset) {
  DEFINEMETHODNAME;
  const BYTE baseIndex = base.getIndex(), inxIndex = inx.getIndex();
  if((inxIndex&7)==4) {
    throwInvalidArgumentException(method, _T("Invalid index register:%s"), inx.getName().cstr());
  }
  if(shift > 3) {
    throwInvalidArgumentException(method, _T("shift=%d. Valid range=[0;3]"),shift);
  }
  if((offset == 0) && ((baseIndex&7) != 5)) {
    setModeBits(MR_SIB(DP_0BYTE)).add(SIB_BYTE(baseIndex,inxIndex,shift));
  } else if(isByte(offset)) {
    setModeBits(MR_SIB(DP_1BYTE)).add(SIB_BYTE(baseIndex,inxIndex,shift)).add((char)offset);
  } else {
    setModeBits(MR_SIB(DP_4BYTE)).add(SIB_BYTE(baseIndex,inxIndex,shift)).add(&offset, 4);
  }
  SETREXBITSONHIGHINX2(baseIndex,inxIndex);
  return *this;
}

InstructionBuilder &InstructionBuilder::prefixSegReg(const SegmentRegister &reg) {
  static const BYTE segRegPrefix[] = { 0x26,0x2e,0x36,0x3e,0x64,0x65 }; // es,cs,ss,ds,fs,gs
  assert(reg.getIndex() < ARRAYSIZE(segRegPrefix));
  return prefix(segRegPrefix[reg.getIndex()]);
}

InstructionBuilder &InstructionBuilder::setRegisterOperand(const Register &reg) {
  const BYTE regIndex = reg.getIndex();
  switch(reg.getType()) {
  case REGTYPE_GPR:
    setOperandSize(reg.getSize());
    setModeBits(MR_REG(regIndex));
    SETREXBITS(HIGHINDEXTOREX(regIndex,REX_B))
    SETREXUNIFORMREGISTER(reg);
    break;
  case REGTYPE_FPU:
    or(regIndex);
    break;
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("reg=%s"), reg.getName().cstr());
  }
  return *this;
}

InstructionBuilder &InstructionBuilderNoMode::setRegisterOperand(const Register &reg) {
  if(!reg.isGPR()) {
    return __super::setRegisterOperand(reg);
  } else {
    const BYTE regIndex = reg.getIndex();
    setOperandSize(reg.getSize());
    or(regIndex&7);
    SETREXBITS(HIGHINDEXTOREX(regIndex,REX_B))
    SETREXUNIFORMREGISTER(reg);
    return *this;
  }
}

InstructionBuilder &InstructionBuilder::setMemoryOperand(const MemoryOperand &mem) {
  const MemoryRef &mr = mem.getMemoryReference();
  if(mem.hasSegmentRegister()) {
    prefixSegReg(*mem.getSegmentRegister());
  }
  if(mr.isDisplaceOnly()) {
    if(getFlags() & IMMMADDR_ALLOWED) {
      addImmAddr(mr.getAddr());
    } else {
      addrDisplaceOnly(mr.getOffset());
    }
  } else if(mr.hasInx()) {
    if(mr.hasBase()) {
      addrBaseShiftInx(*mr.getBase(), *mr.getInx(), mr.getShift(), mr.getOffset());
    } else if(mr.hasShift()) {
      addrShiftInx(*mr.getInx(), mr.getShift(), mr.getOffset());
    } else { // no base, no shift
      addrBase(*mr.getInx(), mr.getOffset());
    }
  } else {
    assert(!mr.hasShift());
    addrBase(*mr.getBase(), mr.getOffset());
  }
  if(!hasOp1RegOnly()) {
    setOperandSize(mem.getSize());
  }
  return *this;
}

InstructionBuilder &InstructionBuilder::setMemOrRegOperand(const InstructionOperand &op) {
  switch(op.getType()) {
  case REGISTER     : return setRegisterOperand(op.getRegister());
  case MEMORYOPERAND: return setMemoryOperand((MemoryOperand&)op);
  default           : throwInvalidArgumentException(__TFUNCTION__, _T("op=%s"),op.toString().cstr());
  }
  return *this;
}

InstructionBuilder &InstructionBuilder::setMemoryRegOperands(const MemoryOperand &mem, const Register &reg) {
  setMemoryOperand(mem);
  if(hasOp1RegOnly() || (mem.getSize() == REGSIZE_VOID)) {
    setOperandSize(reg.getSize());
  }
  if(modeByteCreated()) {
    const BYTE regIndex = reg.getIndex();
    setModeBits((regIndex&7)<<3);
    SETREXBITS(HIGHINDEXTOREX(regIndex,REX_R));
    SETREXUNIFORMREGISTER(reg);
  } else {
    assert(reg.getIndex() == 0);
  }
  return *this;
}

InstructionBuilder &InstructionBuilder::setRegRegOperands(const Register &reg1, const Register &reg2) {
  const BYTE reg1Index = reg1.getIndex();
  const BYTE reg2Index = reg2.getIndex();
  if(reg1.getType() != REGTYPE_GPR) {
    setDirBit1().setOperandSize(reg2.getSize()).setModeBits(MR_REGREG(reg1Index,reg2Index));
    SETREXBITS(HIGHINDEXTOREX(reg1Index,REX_R) | HIGHINDEXTOREX(reg2Index,REX_B))
  } else if(hasOp1RegOnly()) {
    setOperandSize(reg1.getSize()).setModeBits(MR_REGREG(reg1Index,reg2Index));
    SETREXBITS(HIGHINDEXTOREX(reg1Index,REX_R) | HIGHINDEXTOREX(reg2Index,REX_B))
  } else {
    setOperandSize(reg1.getSize()).setModeBits(MR_REGREG(reg2Index,reg1Index));
    SETREXBITS(HIGHINDEXTOREX(reg2Index,REX_R) | HIGHINDEXTOREX(reg1Index,REX_B))
  }
  SETREXUNIFORMREGISTER(reg1);
  SETREXUNIFORMREGISTER(reg2);
  return *this;
}

InstructionBuilder &InstructionBuilder::setImmediateOperand(const InstructionOperand &imm, OperandSize dstSize) {
  if(!hasImmXBit()) {
    return addImmediateOperand(imm,dstSize);
  }
  switch(imm.getSize()) {
  case REGSIZE_BYTE :
    assert(getFlags() & IMM8_ALLOWED);
    if(dstSize != REGSIZE_BYTE) {
      setImmXBit();
    }
    add(imm.getImmInt8());
    break;
  case REGSIZE_WORD :
    if(dstSize == REGSIZE_WORD) {
      assert(getFlags() & IMM16_ALLOWED);
      const short immv = imm.getImmInt16();
      add(&immv, 2);
      break;
    }
    // else continue case
  case REGSIZE_DWORD:
    { assert(getFlags() & IMM32_ALLOWED);
      const int immv = imm.getImmInt32();
      add(&immv, 4);
      break;
    }
  default           :
    sizeError(__TFUNCTION__,imm.getImmInt64());
  }
  return *this;
}

InstructionBuilder &InstructionBuilder::addImmediateOperand(const InstructionOperand &imm, OperandSize size) {
  if(size == REGSIZE_VOID) {
    switch(getFlags() & IMMEDIATEVALUE_ALLOWED) {
    case IMM8_ALLOWED : size = REGSIZE_BYTE ; break;
    case IMM16_ALLOWED: size = REGSIZE_WORD ; break;
    case IMM32_ALLOWED: size = REGSIZE_DWORD; break;
    default           :
      throwInvalidArgumentException(__TFUNCTION__
                                   ,_T("size=%s. Unable to determine size of immediate operand")
                                   ,::toString(size).cstr());
    }
  }
  switch(size) {
  case REGSIZE_BYTE :
    assert(getFlags() & IMM8_ALLOWED);
    add(imm.getImmInt8());
    break;
  case REGSIZE_WORD :
    { assert(getFlags() & IMM16_ALLOWED);
      const short v = imm.getImmInt16();
      add(&v,2);
    }
    break;
  case REGSIZE_DWORD:
    { assert(getFlags() & IMM32_ALLOWED);
      const int v = imm.getImmInt32();
      add(&v,4);
    }
    break;
#ifdef IS64BIT
  case REGSIZE_QWORD:
    { assert(getFlags() & IMM64_ALLOWED);
      const INT64 v = imm.getImmInt64();
      add(&v,8);
    }
    break;
#endif // IS64BIT
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("size=%s"), ::toString(size).cstr());
  }
  return *this;
}
