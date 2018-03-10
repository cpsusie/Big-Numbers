#include "pch.h"
#include <Math/Expression/OpCode.h>

// ---------------------------------- IntelOpcode ----------------------------

#ifdef IS32BIT
#define SETREXBITS(          bits      )
#define SETREXBITONHIGHREG(  reg,bit   )
#define SETREXBITSONHIGHREG2(reg,addReg)
#else // IS64BIT
#define SETREXBITS(          bits      ) { if(bits) setRexBits(bits); }
#define SETREXBITONHIGHREG(  reg,bit   )                                          \
{ if((reg.getIndex()) > 7) setRexBits(1<<(bit));                                  \
}
#define SETREXBITSONHIGHREG2(reg,addReg)                                          \
{ const BYTE _rexBits = (((reg.getIndex())>>3)&1) | (((addReg.getIndex())>>2)&2); \
  SETREXBITS(_rexBits)                                                            \
}
#endif // IS64BIT

void IntelOpcode::throwRegSizeMismatch(const Register &reg) {
  throwInvalidArgumentException(__TFUNCTION__
                               ,_T("register %s doesn't match other operand %s")
                               ,reg.getName().cstr(), getOpSizeName());
}

IntelOpcode &IntelOpcode::insertByte(BYTE index, BYTE b) {
  assert((m_size < MAX_INSTRUCTIONSIZE) && (index<m_size));
  BYTE *bp = m_byte + index;
  for(BYTE *dst = m_byte + m_size++, *src = dst-1; dst > bp;) {
    *(dst--) = *(src--);
  }
  *bp = b;
  return *this;
}

IntelOpcode &IntelOpcode::addGPReg(const GPRegister &reg) {
  assert(hasGPRegMode());
  setRegSize(reg);
  const BYTE    regIndex = reg.getIndex();
  const RegSize regSize  = reg.getSize();
#ifdef IS64BIT
  const BYTE rexbyte = ((regSize==REGSIZE_QWORD)?8:0)|((regIndex>>1)&4);
  SETREXBITS(rexbyte);
#endif
  switch(regSize) {
  case REGSIZE_BYTE : break;
  case REGSIZE_WORD : wordOp();
    // continue case
  default           : orLast(1);
  }
  return addByte((regIndex&7)<<3);
}

IntelOpcode &IntelOpcode::addXMMReg(const XMMRegister &reg) {
  assert(hasXMMRegMode());
  setRegSize(reg);
  const BYTE    regIndex = reg.getIndex();
  SETREXBITONHIGHREG(  reg,0)
  return addByte((regIndex&7)<<3);
}

#ifdef IS64BIT
IntelOpcode &IntelOpcode::setRexBits(BYTE bits) {
  assert(((bits&0xf0)==0) && hasRexMode());
  if(m_hasRexByte) {
    m_byte[m_rexByteIndex] |= bits; // just add new bits to rex-byte
    return *this;
  }
  m_hasRexByte = true;
  return insertByte(m_rexByteIndex, 0x40|bits);
}
#endif

const TCHAR *IntelOpcode::getOpSizeName() const {
  return m_regSizeDefined ? ::getOpSizeName(getOpSize()) : _T("Undefined");
}

// ---------------------------------- IntelInstruction ----------------------------

IntelInstruction &IntelInstruction::add(INT64 bytesToAdd, BYTE count) {
  assert(m_size+count <= MAX_INSTRUCTIONSIZE);
  if(count == 1) {
    m_byte[m_size++] = (BYTE)bytesToAdd;
  } else {
    for(BYTE *dst = m_byte+m_size, *src = (BYTE*)&bytesToAdd, *end = src+count; src<end;) {
      *(dst++) = *(src++);
    }
    m_size += count;
  }
  return *this;
}

IntelInstruction &IntelInstruction::setGPReg(const GPRegister &reg) {
  assert(hasGPRegMode());
  setRegSize(reg);
  const BYTE    regIndex = reg.getIndex();
  const RegSize regSize  = reg.getSize();
#ifdef IS64BIT
  const BYTE rexbyte = ((regSize==REGSIZE_QWORD)?8:0)|((regIndex>>1)&4);
  SETREXBITS(rexbyte);
#endif
  switch(regSize) {
  case REGSIZE_BYTE : break;
  case REGSIZE_WORD : wordOp();
    // continue case
  default           : or(8);
  }
  return or(regIndex&7);
}

IntelInstruction &IntelInstruction::memAddrEsp(int offset) {
  assert(hasMemAddrMode());
  if(offset == 0   ) return memAddrEsp0();
  if(isByte(offset)) return memAddrEsp1((char)offset);
  return memAddrEsp4(offset);
}

IntelInstruction &IntelInstruction::memAddrPtr0(const IndexRegister &reg) {
  assert(hasMemAddrMode());
  const BYTE regIndex = reg.getIndex();
  assert(((regIndex&7)!=4) && ((regIndex&7)!=5));
  SETREXBITONHIGHREG(reg,0);
  return or(regIndex&7);
}

// ptr[reg+offset], (reg&7) != 4, offset=[-128;127]
IntelInstruction &IntelInstruction::memAddrPtr1(const IndexRegister &reg, char offset) {
  assert(hasMemAddrMode());
  const BYTE regIndex = reg.getIndex();
  assert((regIndex&7)!=4);
  SETREXBITONHIGHREG(reg,0);
  return or(0x40 | (regIndex&7)).add(offset);
}

// ptr[reg+offset], (reg&7) != 4, offset=[INT_MIN;INT_MAX]
IntelInstruction &IntelInstruction::memAddrPtr4(const IndexRegister &reg, int offset) {
  assert(hasMemAddrMode());
  const BYTE regIndex = reg.getIndex();
  assert((regIndex&7)!=4);
  SETREXBITONHIGHREG(reg,0);
  return or(0x80 | (regIndex&7)).add(offset, 4);
}

// ptr[reg+(addReg<<p2)], (reg&7) != 5, (addReg&7) != 4, p2<=3
IntelInstruction &IntelInstruction::memAddrMp2AddReg0(const IndexRegister &reg, BYTE p2, const IndexRegister &addReg) {
  assert(hasMemAddrMode());
  const BYTE regIndex = reg.getIndex(), addRegIndex = addReg.getIndex();
  assert(((regIndex&7)!=5) && ((addRegIndex&7)!=4) && (p2<=3));
  SETREXBITSONHIGHREG2(reg,addReg);
  return or(0x04).add((p2 << 6) | ((addRegIndex&7) << 3) | (regIndex&7));
}

// ptr[reg+(addReg<<p2)+offset], (addReg&7) != 4, p2<=3, offset=[-128;127]
IntelInstruction &IntelInstruction::memAddrMp2AddReg1(const IndexRegister &reg, BYTE p2, const IndexRegister &addReg, char offset) {
  assert(hasMemAddrMode());
  const BYTE regIndex = reg.getIndex(), addRegIndex = addReg.getIndex();
  assert(((addRegIndex&7)!=4) && (p2<=3));
  SETREXBITSONHIGHREG2(reg,addReg);
  return or(0x44).add((p2 << 6) | ((addRegIndex&7) << 3) | (regIndex&7)).add(offset);
}

// ptr[reg+(addReg<<p2)+offset], (addReg&7) != 4, p2<=3, offset=[INT_MIN;INT_MAX]
IntelInstruction &IntelInstruction::memAddrMp2AddReg4(const IndexRegister &reg, BYTE p2, const IndexRegister &addReg, int offset) {
  assert(hasMemAddrMode());
  const BYTE regIndex = reg.getIndex(), addRegIndex = addReg.getIndex();
  assert(((addRegIndex&7)!=4) && (p2<=3));
  SETREXBITSONHIGHREG2(reg,addReg);
  return or(0x84).add((p2 << 6) | ((addRegIndex&7) << 3) | (regIndex&7)).add(offset, 4);
}

  // ptr[(reg<<p2)+offset], (reg&7) != 4, p2<=3, offset=[INT_MIN;INT_MAX]
IntelInstruction &IntelInstruction::memAddrMp2Ptr4(const IndexRegister &reg, BYTE p2, int offset) {
  assert(hasMemAddrMode());
  const BYTE regIndex = reg.getIndex();
  assert(((regIndex&7)!=4) && (p2<=3));
  SETREXBITONHIGHREG(reg,1);
  return or(0x04).add(0x05 | (p2 << 6) | ((regIndex&7) << 3)).add(offset, 4);
}

IntelInstruction &IntelInstruction::memAddrPtr(const IndexRegister &reg, int offset) {
  assert(hasMemAddrMode());
  const BYTE regIndex = reg.getIndex();
  switch(regIndex&7) {
  case 4 :
    return memAddrEsp(offset);
  default:
    if(offset == 0 && ((regIndex&7)!=5)) return memAddrPtr0(reg);
    if(isByte(offset)) return memAddrPtr1(reg,(char)offset);
    return memAddrPtr4(reg,offset);
  }
}

IntelInstruction &IntelInstruction::memAddrMp2AddReg(const IndexRegister &reg, const IndexRegister &addReg, BYTE p2, int offset) {
  DEFINEMETHODNAME;
  assert(hasMemAddrMode());
  const BYTE regIndex = reg.getIndex(), addRegIndex = addReg.getIndex();
  if((addRegIndex&7)==4) {
    throwInvalidArgumentException(method, _T("Invalid index register:%s"), reg.getName().cstr());
  }
  if(p2 > 3) {
    throwInvalidArgumentException(method, _T("p2=%d. Valid range=[0;3]"),p2);
  }
  if((offset == 0) && ((regIndex&7) != 5)) return memAddrMp2AddReg0(reg,p2,addReg);
  if(isByte(offset))     return memAddrMp2AddReg1(reg,p2,addReg,(char)offset);
  return memAddrMp2AddReg4(reg,p2,addReg,offset);
}

IntelInstruction &IntelInstruction::regReg(const Register &reg) {
  assert(hasRegRegMode() && ((getRegType() == REGTYPE_NONE) || (getRegType() == reg.getType())));
  setRegSize(reg);
  SETREXBITONHIGHREG(reg,0);
  return or(0xc0 | (reg.getIndex()&7));
}

IntelInstruction &IntelInstruction::setGPRegImm(const GPRegister &reg, int immv) {
  DEFINEMETHODNAME;
  assert(hasGPRegMode() && isImmMode());
  const BYTE    regIndex = reg.getIndex();
  const RegSize regSize  = reg.getSize();
  setRegSize(reg);
  switch(regSize) {
  case REGSIZE_BYTE :
    if(!isByte(immv)) {
      throwInvalidArgumentException(method,_T("immediate value %08x doesn't fit in %s"),immv, reg.getName().cstr());
    }
    if(regIndex != 0) {
      or(regIndex&7).prefix(0x80);
    }
    add((char)immv);
    break;
  case REGSIZE_WORD :
    if(!isWord(immv)) {
      throwInvalidArgumentException(method,_T("immediate value %08x doesn't fit in %s"),immv, reg.getName().cstr());
    }
    if(regIndex != 0) {
      or(regIndex&7).prefix(isByte(immv)?0x83:0x81);
      add(immv,isByte(immv)?1:2);
    } else {
      or(1).add(immv,2);
    }
    wordOp();
    break;
  default           :
    if(regIndex != 0) {
      or(regIndex&7).prefix(isByte(immv)?0x83:0x81);
      add(immv,isByte(immv)?1:4);
    } else {
      or(1).add(immv,4);
    }
    break;
  }
#ifdef IS64BIT
  const BYTE rexbyte = ((regSize==REGSIZE_QWORD)?8:0)|((regIndex>>3)&1);
  SETREXBITS(rexbyte);
#endif
  return *this;
}

IntelInstruction &IntelInstruction::prefixSegReg(const SegmentRegister &reg) {
  static const BYTE segRegPrefix[] = { 0x26,0x2e,0x36,0x3e,0x64,0x65 }; // es,cs,ss,ds,fs,gs
  assert(reg.getIndex() < ARRAYSIZE(segRegPrefix));
  prefix(segRegPrefix[reg.getIndex()]);
  return *this;
}

IntelInstruction &IntelInstruction::setMovGPRegImm(const GPRegister &reg, MovMaxImmType immv) {
  DEFINEMETHODNAME;
  assert(hasGPRegMode() && isImmMode());
  const BYTE    regIndex = reg.getIndex();
  const RegSize regSize  = reg.getSize();
  setRegSize(reg);
  switch(regSize) {
  case REGSIZE_BYTE :
    if(!isByte(immv)) {
      throwInvalidArgumentException(method,_T("immediate value %08I64x doesn't fit in %s"),immv, reg.getName().cstr());
    }
    or(regIndex&7).add((char)immv);
    break;
  case REGSIZE_WORD :
    if(!isWord(immv)) {
      throwInvalidArgumentException(method,_T("immediate value %08I64x doesn't fit in %s"),immv, reg.getName().cstr());
    }
    wordOp();
    or(8 | (regIndex&7)).add(immv,2);
    break;
  case REGSIZE_DWORD :
    if(!isDword(immv)) {
      throwInvalidArgumentException(method,_T("immediate value %08I64x doesn't fit in %s"),immv, reg.getName().cstr());
    }
    or(8 | (regIndex&7)).add(immv,4);
    break;
#ifdef IS64BIT
  case REGSIZE_QWORD :
    or(8 | (regIndex&7)).add(immv,8);
    break;
#endif
  default           :
    throwInvalidArgumentException(method,_T("Unknown registersize:%d. reg=%s"), regSize, reg.getName().cstr());
    break;
  }
#ifdef IS64BIT
  const BYTE rexbyte = ((regSize==REGSIZE_QWORD)?8:0)|((regIndex>>3)&1);
  SETREXBITS(rexbyte);
#endif
  return *this;
}
