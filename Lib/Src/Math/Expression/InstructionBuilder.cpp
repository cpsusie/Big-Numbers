#include "pch.h"
#include "InstructionBuilder.h"

const RegSizeSet InstructionBuilder::s_sizeBitSet(REGSIZE_WORD ,REGSIZE_DWORD ,REGSIZE_QWORD, REGSIZE_END);

InstructionBuilder::InstructionBuilder(const OpcodeBase &opcode)
  : InstructionBase(opcode               )
  , m_extension(    opcode.getExtension())
  , m_opcodeSize(   opcode.size()        )
  , m_opCount(      opcode.getOpCount()  )
{
/*
#ifdef _DEBUG
  if(m_extension) {
    if(m_opCount > 1) {
      throwInvalidArgumentException(__TFUNCTION__,_T("extension=%d, opCount=%d"), m_extension, m_opCount);
    }
  }
#endif // _DEBUG
*/
  init();
  if(m_extension) {
    addExtension();
  }
}

InstructionBuilder::InstructionBuilder(const InstructionBase &ins)
  : InstructionBase(ins       )
  , m_extension(    0         )
  , m_opcodeSize(   ins.size())
  , m_opCount(      0         )
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

InstructionBuilder &InstructionBuilder::add(const BYTE *src, BYTE count) {
  assert(m_size+count <= MAX_INSTRUCTIONSIZE);
  if(count == 1) {
    m_bytes[m_size++] = *src;
  } else {
    for(BYTE *dst = m_bytes+m_size, *end = dst+count; dst<end;) {
      *(dst++) = *(src++);
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
    setSizeBit();
    SETREXBITS(QWORDTOREX(REGSIZE_QWORD));
  }
  return *this;
}

InstructionBuilder &InstructionBuilder::setModeBits(BYTE bits) {
  if(m_hasModeByte) {
    return or(getModeByteIndex(), bits);
  } else {
    m_hasModeByte = true;
    return add(bits);
  }
}

InstructionBuilder &InstructionBuilder::prefixImm(BYTE b, OperandSize size, bool immIsByte) {
  if(needSizeBit(size)) b |= 1;
  if((size != REGSIZE_BYTE) && immIsByte) b |= 2;
  prefix(b);
  m_hasModeByte = true;
  m_opcodePos--;
  switch(size) {
  case REGSIZE_WORD:
    wordIns();
    break;
  case REGSIZE_QWORD:
    SETREXBITS(QWORDTOREX(REGSIZE_QWORD));
    break;
  }
  return *this;
}

// ptr[(inx<<shift)+offset], (inx&7) != 4, shift<=3, offset=[INT_MIN;INT_MAX]
InstructionBuilder &InstructionBuilder::addrShiftInx(const IndexRegister &inx, BYTE shift, int offset) {
  const BYTE inxIndex = inx.getIndex();
  assert(((inxIndex&7)!=4) && (shift<=3));
  SETREXBITONHIGHINX(inxIndex,1);
  return setModeBits(MR_SIB(0)).add(SIB_BYTE(5,inxIndex,shift)).add((BYTE*)&offset, 4);
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
      setModeBits(MR_SIB(DP_4BYTE)).add(0x24).add((BYTE*)&offset, 4); // ptr[esp+4 byte offset]
    }
    break;
  default:
    if((offset == 0) && ((baseIndex&7)!=5)) {
      setModeBits(baseIndex&7);                                       // ptr[base],               (base&7) != {4,5}
    } else if(isByte(offset)) {
      setModeBits(MR_DP1BYTE(baseIndex)).add((char)offset);           // ptr[base+1 byte offset], (base&7) != 4
    } else {
      setModeBits(MR_DP4BYTE(baseIndex)).add((BYTE*)&offset, 4);      // ptr[base+4 byte offset], (base&7) != 4
    }
  }
  SETREXBITONHIGHINX(baseIndex,0);
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
    setModeBits(MR_SIB(DP_4BYTE)).add(SIB_BYTE(baseIndex,inxIndex,shift)).add((BYTE*)&offset, 4);
  }
  SETREXBITSONHIGHINX2(baseIndex,inxIndex);
  return *this;
}

InstructionBuilder &InstructionBuilder::prefixSegReg(const SegmentRegister &reg) {
  static const BYTE segRegPrefix[] = { 0x26,0x2e,0x36,0x3e,0x64,0x65 }; // es,cs,ss,ds,fs,gs
  assert(reg.getIndex() < ARRAYSIZE(segRegPrefix));
  return prefix(segRegPrefix[reg.getIndex()]);
}

InstructionBuilder &InstructionBuilder::setRegisterOperand(const GPRegister &reg) {
  const BYTE    index = reg.getIndex();
  const RegSize size  = reg.getSize();
  setOperandSize(size).setModeBits(MR_REG(index));
  SETREXBITS(HIGHINDEXTOREX(index,0))
  return *this;
}

InstructionBuilder &InstructionBuilder::setMemoryOperand(const MemoryOperand &mem) {
  const MemoryRef &mr = *mem.getMemoryReference();
  if(mem.hasSegmentRegister()) {
    prefixSegReg(mem.getSegmentRegister());
  }
  if(mr.isDisplaceOnly()) {
    addrDisplaceOnly(mr.getOffset());
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
  return setOperandSize(mem.getSize());
}
