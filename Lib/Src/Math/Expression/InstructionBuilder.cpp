#include "pch.h"
#include "InstructionBuilder.h"
// --------------------------------- InstructionBase -----------------------------------

const RegSizeSet InstructionBuilder::s_sizeBitSet(REGSIZE_WORD ,REGSIZE_DWORD ,REGSIZE_QWORD, REGSIZE_END);

InstructionBuilder::InstructionBuilder(const OpcodeBase &opcode)
  : InstructionBase(opcode             )
  , m_opcodeSize(   opcode.size()      )
  , m_opCount(      opcode.getOpCount())
{
  init();
}

InstructionBuilder::InstructionBuilder(const Instruction0Arg &ins0)
  : InstructionBase(ins0       )
  , m_opcodeSize(   ins0.size())
  , m_opCount(      0          )
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
#endif

InstructionBuilder &InstructionBuilder::add(INT64 bytesToAdd, BYTE count) {
  assert(m_size+count <= MAX_INSTRUCTIONSIZE);
  if(count == 1) {
    m_bytes[m_size++] = (BYTE)bytesToAdd;
  } else {
    for(BYTE *dst = m_bytes+m_size, *src = (BYTE*)&bytesToAdd, *end = src+count; src<end;) {
      *(dst++) = *(src++);
    }
    m_size += count;
  }
  return *this;
}

// ptr[(inx<<shift)+offset], (inx&7) != 4, shift<=3, offset=[INT_MIN;INT_MAX]
InstructionBuilder &InstructionBuilder::addrShiftInx(const IndexRegister &inx, BYTE shift, int offset) {
  const BYTE inxIndex = inx.getIndex();
  assert(((inxIndex&7)!=4) && (shift<=3));
  SETREXBITONHIGHINX(inxIndex,1);
  return or(MR_SIB(0)).add(SIB_BYTE(5,inxIndex,shift)).add(offset, 4);
}

// ------------------------------------------------------------------------------------------

InstructionBuilder &InstructionBuilder::addrBase(const IndexRegister &base, int offset) {
  const BYTE baseIndex = base.getIndex();
  switch(baseIndex&7) {
  case 4 :
    if(offset == 0) {
      or(MR_SIB(DP_0BYTE)).add(0x24);                        // ptr[esp]
    } else if(isByte(offset)) {
      or(MR_SIB(DP_1BYTE)).add(0x24).add((char)offset);      // ptr[esp+1 byte offset] 
    } else {
      or(MR_SIB(DP_4BYTE)).add(0x24).add(offset, 4);         // ptr[esp+4 byte offset]
    }
    break;
  default:
    if((offset == 0) && ((baseIndex&7)!=5)) {
      or(baseIndex&7);                                       // ptr[base],               (base&7) != {4,5}
    } else if(isByte(offset)) {
      or(MR_DP1BYTE(baseIndex)).add((char)offset);           // ptr[base+1 byte offset], (base&7) != 4
    } else {
      or(MR_DP4BYTE(baseIndex)).add(offset, 4);              // ptr[base+4 byte offset], (base&7) != 4
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
    or(MR_SIB(DP_0BYTE)).add(SIB_BYTE(baseIndex,inxIndex,shift));
  } else if(isByte(offset)) {
    or(MR_SIB(DP_1BYTE)).add(SIB_BYTE(baseIndex,inxIndex,shift)).add((char)offset);
  } else {
    or(MR_SIB(DP_4BYTE)).add(SIB_BYTE(baseIndex,inxIndex,shift)).add(offset, 4);
  }
  SETREXBITSONHIGHINX2(baseIndex,inxIndex);
  return *this;
}

InstructionBuilder &InstructionBuilder::prefixSegReg(const SegmentRegister &reg) {
  static const BYTE segRegPrefix[] = { 0x26,0x2e,0x36,0x3e,0x64,0x65 }; // es,cs,ss,ds,fs,gs
  assert(reg.getIndex() < ARRAYSIZE(segRegPrefix));
  prefix(segRegPrefix[reg.getIndex()]);
  return *this;
}

InstructionBuilder &InstructionBuilder::setMemoryOperand(const MemoryOperand &mop) {
  const MemoryRef &mr = *mop.getMemoryReference();
  if(mop.hasSegmentRegister()) {
    prefixSegReg(mop.getSegmentRegister());
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
  return *this;
}

InstructionBuilder &InstructionBuilder::addMemoryOperand(const MemoryOperand &mop) {
  return add(0).setMemoryOperand(mop);
}

void InstructionBuilder::sizeError(const TCHAR *method, const GPRegister    &reg, INT64 immv) { // static
  throwInvalidArgumentException(method,_T("Immediate value %s doesn't fit in %s"),formatHexValue(immv,false).cstr(), reg.getName().cstr());
}

void InstructionBuilder::sizeError(const TCHAR *method, const MemoryOperand &memop, INT64 immv) { // static
  throwInvalidArgumentException(method,_T("Immediate value %s doesn't fit in %s"),formatHexValue(immv,false).cstr(), memop.toString().cstr());
}
