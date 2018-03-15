#include "pch.h"
#include <Math/Expression/NewOpCode.h>
#include "RexByte.h"
// --------------------------------- InstructionBase -----------------------------------

InstructionBase::InstructionBase(const OpcodeBase &opcode)
  : m_size(opcode.size())
{
  memcpy(m_bytes, opcode.getBytes(),m_size);
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

// ptr[(reg<<shift)+offset], (reg&7) != 4, shift<=3, offset=[INT_MIN;INT_MAX]
InstructionBuilder &InstructionBuilder::addrShiftPtr4(const IndexRegister &reg, BYTE shift, int offset) {
  const BYTE regIndex = reg.getIndex();
  assert(((regIndex&7)!=4) && (shift<=3));
  SETREXBITONHIGHINX(regIndex,1);
  return or(0x04).add(0x05 | (shift << 6) | ((regIndex&7) << 3)).add(offset, 4);
}

// ------------------------------------------------------------------------------------------

InstructionBuilder &InstructionBuilder::addrPtr(const IndexRegister &reg, int offset) {
  const BYTE regIndex = reg.getIndex();
  switch(regIndex&7) {
  case 4 :
    if(offset == 0) {
      or(0x04).add(0x24);                        // ptr[esp]
    } else if(isByte(offset)) {
      or(0x44).add(0x24).add((char)offset);      // ptr[esp+1 byte offset] 
    } else {
      or(0x84).add(0x24).add(offset, 4);         // ptr[esp+4 byte offset]
    }
    break;
  default:
    if((offset == 0) && ((regIndex&7)!=5)) {
      or(regIndex&7);                            // ptr[reg]. (reg&7) != {4,5}
    } else if(isByte(offset)) {
      or(0x40 | (regIndex&7)).add((char)offset); // ptr[reg+1 byte offset], (reg&7) != 4
    } else {
      or(0x80 | (regIndex&7)).add(offset, 4);    // ptr[reg+4 byte offset], (reg&7) != 4
    }
  }
  SETREXBITONHIGHINX(regIndex,0);
  return *this;
}

InstructionBuilder &InstructionBuilder::addrShiftAddReg(const IndexRegister &base, BYTE shift, const IndexRegister &inxReg, int offset) {
  DEFINEMETHODNAME;
  const BYTE baseIndex = base.getIndex(), inxRegIndex = inxReg.getIndex();
  if((inxRegIndex&7)==4) {
    throwInvalidArgumentException(method, _T("Invalid index register:%s"), inxReg.getName().cstr());
  }
  if(shift > 3) {
    throwInvalidArgumentException(method, _T("shift=%d. Valid range=[0;3]"),shift);
  }
  if((offset == 0) && ((baseIndex&7) != 5)) {
    or(0x04).add((shift << 6) | ((inxRegIndex&7) << 3) | (baseIndex&7));
  } else if(isByte(offset)) {
    or(0x44).add((shift << 6) | ((inxRegIndex&7) << 3) | (baseIndex&7)).add((char)offset);
  } else {
    or(0x84).add((shift << 6) | ((inxRegIndex&7) << 3) | (baseIndex&7)).add(offset, 4);
  }
  SETREXBITSONHIGHINX2(baseIndex,inxRegIndex);
  return *this;
}

InstructionBuilder &InstructionBuilder::prefixSegReg(const SegmentRegister &reg) {
  static const BYTE segRegPrefix[] = { 0x26,0x2e,0x36,0x3e,0x64,0x65 }; // es,cs,ss,ds,fs,gs
  assert(reg.getIndex() < ARRAYSIZE(segRegPrefix));
  prefix(segRegPrefix[reg.getIndex()]);
  return *this;
}

InstructionBuilder &InstructionBuilder::setMemoryReference(const MemoryOperand &mop) {
  const MemoryRef &mr = *mop.getMemoryReference();
  if(mop.hasSegmentRegister()) {
    prefixSegReg(mop.getSegmentRegister());
  }
  if(mr.isImmediateAddr()) {
    addrImmDword(mr.getOffset());
  } else if(mr.getAddreg()) {
    addrShiftAddReg(*mr.getReg(), mr.getShift(), *mr.getAddreg(), mr.getOffset());
  } else if(mr.hasShift()) {
    addrShiftPtr4(*mr.getAddreg(), mr.getShift(), mr.getOffset());
  } else {
    addrPtr(*mr.getReg(), mr.getOffset());
  }
  return *this;
}

InstructionBuilder &InstructionBuilder::addMemoryReference(const MemoryOperand &mop) {
  return add(0).setMemoryReference(mop);
}
