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

InstructionBuilder &InstructionBuilder::addrStack(int offset) {
  if(offset == 0) return addrStack0();
  if(isByte(offset)) return addrStack1((char)offset);
  return addrStack4(offset);
}

InstructionBuilder &InstructionBuilder::addrPtr0(const IndexRegister &reg) {
  const BYTE regIndex = reg.getIndex();
  assert(((regIndex&7)!=4) && ((regIndex&7)!=5));
  SETREXBITONHIGHREG(reg,0);
  return add(regIndex&7);
}

// ptr[reg+offset], (reg&7) != 4, offset=[-128;127]
InstructionBuilder &InstructionBuilder::addrPtr1(const IndexRegister &reg, char offset) {
  const BYTE regIndex = reg.getIndex();
  assert((regIndex&7)!=4);
  SETREXBITONHIGHREG(reg,0);
  return add(0x40 | (regIndex&7)).add(offset);
}

// ptr[reg+offset], (reg&7) != 4, offset=[INT_MIN;INT_MAX]
InstructionBuilder &InstructionBuilder::addrPtr4(const IndexRegister &reg, int offset) {
  const BYTE regIndex = reg.getIndex();
  assert((regIndex&7)!=4);
  SETREXBITONHIGHREG(reg,0);
  return add(0x80 | (regIndex&7)).add(offset, 4);
}

// ptr[reg+(addReg<<shift)], (reg&7) != 5, (addReg&7) != 4, shift<=3
InstructionBuilder &InstructionBuilder::addrShiftAddReg0(const IndexRegister &reg, BYTE shift, const IndexRegister &addReg) {
  const BYTE regIndex = reg.getIndex(), addRegIndex = addReg.getIndex();
  assert(((regIndex&7)!=5) && ((addRegIndex&7)!=4) && (shift<=3));
  SETREXBITSONHIGHREG2(reg,addReg);
  return add(0x04).add((shift << 6) | ((addRegIndex&7) << 3) | (regIndex&7));
}

// ptr[reg+(addReg<<shift)+offset], (addReg&7) != 4, shift<=3, offset=[-128;127]
InstructionBuilder &InstructionBuilder::addrShiftAddReg1(const IndexRegister &reg, BYTE shift, const IndexRegister &addReg, char offset) {
  const BYTE regIndex = reg.getIndex(), addRegIndex = addReg.getIndex();
  assert(((addRegIndex&7)!=4) && (shift<=3));
  SETREXBITSONHIGHREG2(reg,addReg);
  return add(0x44).add((shift << 6) | ((addRegIndex&7) << 3) | (regIndex&7)).add(offset);
}

// ptr[reg+(addReg<<shift)+offset], (addReg&7) != 4, shift<=3, offset=[INT_MIN;INT_MAX]
InstructionBuilder &InstructionBuilder::addrShiftAddReg4(const IndexRegister &reg, BYTE shift, const IndexRegister &addReg, int offset) {
  const BYTE regIndex = reg.getIndex(), addRegIndex = addReg.getIndex();
  assert(((addRegIndex&7)!=4) && (shift<=3));
  SETREXBITSONHIGHREG2(reg,addReg);
  return add(0x84).add((shift << 6) | ((addRegIndex&7) << 3) | (regIndex&7)).add(offset, 4);
}

  // ptr[(reg<<shift)+offset], (reg&7) != 4, shift<=3, offset=[INT_MIN;INT_MAX]
InstructionBuilder &InstructionBuilder::addrShiftPtr4(const IndexRegister &reg, BYTE shift, int offset) {
  const BYTE regIndex = reg.getIndex();
  assert(((regIndex&7)!=4) && (shift<=3));
  SETREXBITONHIGHREG(reg,1);
  return add(0x04).add(0x05 | (shift << 6) | ((regIndex&7) << 3)).add(offset, 4);
}

// ------------------------------------------------------------------------------------------
InstructionBuilder &InstructionBuilder::addrPtr(const IndexRegister &reg, int offset) {
  const BYTE regIndex = reg.getIndex();
  switch(regIndex&7) {
  case 4 :
    return addrStack(offset);
  default:
    if((offset == 0) && ((regIndex&7)!=5)) return addrPtr0(reg);
    if(isByte(offset)) return addrPtr1(reg,(char)offset);
    return addrPtr4(reg,offset);
  }
}

InstructionBuilder &InstructionBuilder::addrShiftAddReg(const IndexRegister &reg, const IndexRegister &addReg, BYTE shift, int offset) {
  DEFINEMETHODNAME;
  const BYTE regIndex = reg.getIndex(), addRegIndex = addReg.getIndex();
  if((addRegIndex&7)==4) {
    throwInvalidArgumentException(method, _T("Invalid index register:%s"), reg.getName().cstr());
  }
  if(shift > 3) {
    throwInvalidArgumentException(method, _T("shift=%d. Valid range=[0;3]"),shift);
  }
  if((offset == 0) && ((regIndex&7) != 5)) return addrShiftAddReg0(reg,shift,addReg);
  if(isByte(offset))     return addrShiftAddReg1(reg,shift,addReg,(char)offset);
  return addrShiftAddReg4(reg,shift,addReg,offset);
}

InstructionBuilder &InstructionBuilder::prefixSegReg(const SegmentRegister &reg) {
  static const BYTE segRegPrefix[] = { 0x26,0x2e,0x36,0x3e,0x64,0x65 }; // es,cs,ss,ds,fs,gs
  assert(reg.getIndex() < ARRAYSIZE(segRegPrefix));
  prefix(segRegPrefix[reg.getIndex()]);
  return *this;
}

InstructionBuilder &InstructionBuilder::addMemoryReference(const MemoryOperand &mop) {
  const MemoryRef &mr = *mop.getMemoryReference();
  if(mop.hasSegmentRegister()) {
    prefixSegReg(mop.getSegmentRegister());
  }
  if(mr.isImmediateAddr()) {
    addrImmDword(mr.getOffset());
  } else if(mr.getAddreg()) {
    addrShiftAddReg(*mr.getReg(), *mr.getAddreg(), mr.getShift(), mr.getOffset());
  } else if(mr.hasShift()) {
    addrShiftPtr4(*mr.getAddreg(), mr.getShift(), mr.getOffset());
  } else {
    addrPtr(*mr.getReg(), mr.getOffset());
  }
  return *this;
}
