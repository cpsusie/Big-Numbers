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

const TCHAR *IntelOpcode::getOpSizeName(RegSize regSize) { // static
  static const TCHAR *name[] = { _T("BYTE"), _T("WORD"), _T("DWORD"), _T("QWORD"), _T("TBYTE"), _T("OWORD") };
  return name[regSize];
}

const TCHAR *IntelOpcode::getOpSizeName() const {
  return m_regSizeDefined ? getOpSizeName(getOpSize()) : _T("Undefined");
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
  if(isByte(offset))                       return memAddrMp2AddReg1(reg,p2,addReg,(char)offset);
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

// ---------------------------------- Register ----------------------------

String GPRegister::getName() const {
  switch(getSize()) {
  case REGSIZE_BYTE :
    switch(getIndex()) {
    case  0: return _T("AL"  );
    case  1: return _T("CL"  );
    case  2: return _T("DL"  );
    case  3: return _T("BL"  );
    case  4: return _T("AH"  );
    case  5: return _T("CH"  );
    case  6: return _T("DH"  );
    case  7: return _T("BH"  );
#ifdef IS64BIT
    case  8:
    case  9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
      return format(_T("R%dB"),getIndex());
#endif // IS64BIT
    }
    break;
  case REGSIZE_WORD :
    switch(getIndex()) {
    case  0: return _T("AX"  );
    case  1: return _T("CX"  );
    case  2: return _T("DX"  );
    case  3: return _T("BX"  );
    case  4: return _T("SP"  );
    case  5: return _T("BP"  );
    case  6: return _T("SI"  );
    case  7: return _T("DI"  );
#ifdef IS64BIT
    case  8:
    case  9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
      return format(_T("R%dW"),getIndex());
#endif // IS64BIT
    }
    break;
  case REGSIZE_DWORD:
    switch(getIndex()) {
    case  0: return _T("EAX" );
    case  1: return _T("ECX" );
    case  2: return _T("EDX" );
    case  3: return _T("EBX" );
    case  4: return _T("ESP" );
    case  5: return _T("EBP" );
    case  6: return _T("ESI" );
    case  7: return _T("EDI" );
#ifdef IS64BIT
    case  8:
    case  9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
      return format(_T("R%dD"), getIndex());
#endif // IS64BIT
    }
    break;
#ifdef IS64BIT
  case REGSIZE_QWORD:
    switch(getIndex()) {
    case  0: return _T("RAX" );
    case  1: return _T("RCX" );
    case  2: return _T("RDX" );
    case  3: return _T("RBX" );
    case  4: return _T("RSP" );
    case  5: return _T("RBP" );
    case  6: return _T("RSI" );
    case  7: return _T("RDI" );
    case  8:
    case  9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
      return format(_T("R%d"),getIndex());
    }
    break;
#endif // IS64BIT
  } // switch(regSize)
  return __super::getName();
}

String SegmentRegister::getName() const {
  switch(getIndex()) {
  case 0 : return _T("ES");
  case 1 : return _T("CS");
  case 2 : return _T("SS");
  case 3 : return _T("DS");
  case 4 : return _T("FS");
  case 5 : return _T("GS");
  }
  return __super::getName();
}

#pragma warning(disable : 4073)
#pragma init_seg(lib)

const GPRegister    AL(REGSIZE_BYTE,  0);
const GPRegister    CL(REGSIZE_BYTE,  1);
const GPRegister    DL(REGSIZE_BYTE,  2);
const GPRegister    BL(REGSIZE_BYTE,  3);
const GPRegister    AH(REGSIZE_BYTE,  4);
const GPRegister    CH(REGSIZE_BYTE,  5);
const GPRegister    DH(REGSIZE_BYTE,  6);
const GPRegister    BH(REGSIZE_BYTE,  7);

const GPRegister    AX(REGSIZE_WORD,  0);
const GPRegister    CX(REGSIZE_WORD,  1);
const GPRegister    DX(REGSIZE_WORD,  2);
const GPRegister    BX(REGSIZE_WORD,  3);
const GPRegister    SP(REGSIZE_WORD,  4);
const GPRegister    BP(REGSIZE_WORD,  5);
const GPRegister    SI(REGSIZE_WORD,  6);
const GPRegister    DI(REGSIZE_WORD,  7);

#ifdef IS32BIT

const IndexRegister EAX(0);
const IndexRegister ECX(1);
const IndexRegister EDX(2);
const IndexRegister EBX(3);
const IndexRegister ESP(4);
const IndexRegister EBP(5);
const IndexRegister ESI(6);
const IndexRegister EDI(7);

#else // IS64BIT

// 8 bit registers (only x64);
const GPRegister    R8B( REGSIZE_BYTE , 8);
const GPRegister    R9B( REGSIZE_BYTE , 9);
const GPRegister    R10B(REGSIZE_BYTE ,10);
const GPRegister    R11B(REGSIZE_BYTE ,11);
const GPRegister    R12B(REGSIZE_BYTE ,12);
const GPRegister    R13B(REGSIZE_BYTE ,13);
const GPRegister    R14B(REGSIZE_BYTE ,14);
const GPRegister    R15B(REGSIZE_BYTE ,15);

// 16 bit registers (only x64);
const GPRegister    R8W( REGSIZE_WORD , 8);
const GPRegister    R9W( REGSIZE_WORD , 9);
const GPRegister    R10W(REGSIZE_WORD ,10);
const GPRegister    R11W(REGSIZE_WORD ,11);
const GPRegister    R12W(REGSIZE_WORD ,12);
const GPRegister    R13W(REGSIZE_WORD ,13);
const GPRegister    R14W(REGSIZE_WORD ,14);
const GPRegister    R15W(REGSIZE_WORD ,15);

const GPRegister    EAX( REGSIZE_DWORD, 0);
const GPRegister    ECX( REGSIZE_DWORD, 1);
const GPRegister    EDX( REGSIZE_DWORD, 2);
const GPRegister    EBX( REGSIZE_DWORD, 3);
const GPRegister    ESP( REGSIZE_DWORD, 4);
const GPRegister    EBP( REGSIZE_DWORD, 5);
const GPRegister    ESI( REGSIZE_DWORD, 6);
const GPRegister    EDI( REGSIZE_DWORD, 7);
// 32 bit registers (only x64);
const GPRegister    R8D (REGSIZE_DWORD, 8);
const GPRegister    R9D (REGSIZE_DWORD, 9);
const GPRegister    R10D(REGSIZE_DWORD,10);
const GPRegister    R11D(REGSIZE_DWORD,11);
const GPRegister    R12D(REGSIZE_DWORD,12);
const GPRegister    R13D(REGSIZE_DWORD,13);
const GPRegister    R14D(REGSIZE_DWORD,14);
const GPRegister    R15D(REGSIZE_DWORD,15);

// 64 bit registers (only x64);
const IndexRegister RAX( 0);
const IndexRegister RCX( 1);
const IndexRegister RDX( 2);
const IndexRegister RBX( 3);
const IndexRegister RSP( 4);
const IndexRegister RBP( 5);
const IndexRegister RSI( 6);
const IndexRegister RDI( 7);
const IndexRegister R8(  8);
const IndexRegister R9(  9);
const IndexRegister R10(10);
const IndexRegister R11(11);
const IndexRegister R12(12);
const IndexRegister R13(13);
const IndexRegister R14(14);
const IndexRegister R15(15);

#endif // IS64BIT

const FPURegister ST0( 0);
const FPURegister ST1( 1);
const FPURegister ST2( 2);
const FPURegister ST3( 3);
const FPURegister ST4( 4);
const FPURegister ST5( 5);
const FPURegister ST6( 6);
const FPURegister ST7( 7);

const XMMRegister XMM0(  0);
const XMMRegister XMM1(  1);
const XMMRegister XMM2(  2);
const XMMRegister XMM3(  3);
const XMMRegister XMM4(  4);
const XMMRegister XMM5(  5);
const XMMRegister XMM6(  6);
const XMMRegister XMM7(  7);

#ifdef IS64BIT
const XMMRegister XMM8(  8);
const XMMRegister XMM9(  9);
const XMMRegister XMM10(10);
const XMMRegister XMM11(11);
const XMMRegister XMM12(12);
const XMMRegister XMM13(13);
const XMMRegister XMM14(14);
const XMMRegister XMM15(15);
#endif // IS64BIT

const SegmentRegister ES(0);
const SegmentRegister CS(1);
const SegmentRegister SS(2);
const SegmentRegister DS(3);
const SegmentRegister FS(4);
const SegmentRegister GS(5);
