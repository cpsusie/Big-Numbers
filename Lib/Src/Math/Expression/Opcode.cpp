#include "pch.h"
#include <Math/Expression/OpCode.h>

// ---------------------------------- Register ----------------------------

String Register::getName() const {
  switch(getType()) {
  case REGTYPE_SEG:
    switch(getIndex()) {
    case 0: return _T("ES");
    case 1: return _T("CS");
    case 2: return _T("SS");
    case 3: return _T("DS");
    case 4: return _T("FS");
    case 5: return _T("GS");
    }
    break;
  case REGTYPE_FPU:
    switch(getIndex()) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
      return format(_T("ST(%d)"),getIndex());
    }
    break;
  case REGTYPE_XMM:
    switch(getIndex()) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
#ifdef IS64BIT
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
#endif // IS64BIT
      return format(_T("XMM%d"),getIndex());
    }
    break;
  case REGTYPE_GP :
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
    break;
  } // switch(regType)
  return format(_T("Unknown register:(type,sz,index):(%d,%d,%u"), getType(),getSize(),getIndex());
}

// ---------------------------------- IntelOpcode ----------------------------

#ifdef IS32BIT
#define SETREXBITS(bits)
#define SETREXBITONHIGHREG(  reg,bit)
#define SETREXBITSONHIGHREG2(reg,addReg)
#else // IS64BIT
#define SETREXBITS(bits) { if(bits) setRexBits(bits); }
#define SETREXBITONHIGHREG(  reg,bit)                                             \
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

IntelOpcode &IntelOpcode::addGPReg(const Register &reg) {
  assert(hasGPRegMode() && reg.isGPRegister());
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

IntelOpcode &IntelOpcode::addXMMReg(const Register &reg) {
  assert(hasXMMRegMode() && reg.isXMMRegister());
  setRegSize(reg);
  const BYTE    regIndex = reg.getIndex();
  const RegSize regSize  = reg.getSize();
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
  static const TCHAR *name[] = { _T("BYTE"), _T("WORD"), _T("DWORD"), _T("QWORD"), _T("OWORD") };
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

IntelInstruction &IntelInstruction::setGPReg(const Register &reg) {
  assert(hasGPRegMode() && reg.isGPRegister());
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

IntelInstruction &IntelInstruction::memAddrPtr0(const Register &reg) {
  assert(hasMemAddrMode() && reg.isIndexRegister());
  const BYTE    regIndex = reg.getIndex();
  assert(((regIndex&7)!=4) && ((regIndex&7)!=5));
  SETREXBITONHIGHREG(reg,0);
  return or(regIndex&7);
}

// ptr[reg+offset], (reg&7) != 4, offset=[-128;127]
IntelInstruction &IntelInstruction::memAddrPtr1(const Register &reg, char offset) {
  assert(hasMemAddrMode() && reg.isIndexRegister());
  const BYTE    regIndex = reg.getIndex();
  assert((regIndex&7)!=4);
  SETREXBITONHIGHREG(reg,0);
  return or(0x40 | (regIndex&7)).add(offset);
}

// ptr[reg+offset], (reg&7) != 4, offset=[INT_MIN;INT_MAX]
IntelInstruction &IntelInstruction::memAddrPtr4(const Register &reg, int offset) {
  assert(hasMemAddrMode() && reg.isIndexRegister());
  const BYTE    regIndex = reg.getIndex();
  assert((regIndex&7)!=4);
  SETREXBITONHIGHREG(reg,0);
  return or(0x80 | (regIndex&7)).add(offset, 4);
}

// ptr[reg+(addReg<<p2)], (reg&7) != 5, (addReg&7) != 4, p2<=3
IntelInstruction &IntelInstruction::memAddrMp2AddReg0(const Register &reg, BYTE p2, const Register &addReg) {
  assert(hasMemAddrMode() && reg.isIndexRegister() && addReg.isIndexRegister());
  const BYTE    regIndex = reg.getIndex(), addRegIndex = addReg.getIndex();
  assert(((regIndex&7)!=5) && ((addRegIndex&7)!=4) && (p2<=3));
  SETREXBITSONHIGHREG2(reg,addReg);
  return or(0x04).add((p2 << 6) | ((addRegIndex&7) << 3) | (regIndex&7));
}

// ptr[reg+(addReg<<p2)+offset], (addReg&7) != 4, p2<=3, offset=[-128;127]
IntelInstruction &IntelInstruction::memAddrMp2AddReg1(const Register &reg, BYTE p2, const Register &addReg, char offset) {
  assert(hasMemAddrMode() && reg.isIndexRegister() && addReg.isIndexRegister());
  const BYTE    regIndex = reg.getIndex(), addRegIndex = addReg.getIndex();
  assert(((addRegIndex&7)!=4) && (p2<=3));
  SETREXBITSONHIGHREG2(reg,addReg);
  return or(0x44).add((p2 << 6) | ((addRegIndex&7) << 3) | (regIndex&7)).add(offset);
}

// ptr[reg+(addReg<<p2)+offset], (addReg&7) != 4, p2<=3, offset=[INT_MIN;INT_MAX]
IntelInstruction &IntelInstruction::memAddrMp2AddReg4(const Register &reg, BYTE p2, const Register &addReg, int offset) {
  assert(hasMemAddrMode() && reg.isIndexRegister() && addReg.isIndexRegister());
  const BYTE    regIndex = reg.getIndex(), addRegIndex = addReg.getIndex();
  assert(((addRegIndex&7)!=4) && (p2<=3));
  SETREXBITSONHIGHREG2(reg,addReg);
  return or(0x84).add((p2 << 6) | ((addRegIndex&7) << 3) | (regIndex&7)).add(offset, 4);
}

  // ptr[(reg<<p2)+offset], (reg&7) != 4, p2<=3, offset=[INT_MIN;INT_MAX]
IntelInstruction &IntelInstruction::memAddrMp2Ptr4(const Register &reg, BYTE p2, int offset) {
  assert(hasMemAddrMode() && reg.isIndexRegister());
  const BYTE    regIndex = reg.getIndex();
  assert(((regIndex&7)!=4) && (p2<=3));
  SETREXBITONHIGHREG(reg,1);
  return or(0x04).add(0x05 | (p2 << 6) | ((regIndex&7) << 3)).add(offset, 4);
}

IntelInstruction &IntelInstruction::memAddrPtr(const Register &reg, int offset) {
  assert(hasMemAddrMode() && reg.isIndexRegister());
  const BYTE    regIndex = reg.getIndex();
  switch(regIndex&7) {
  case 4 :
    return memAddrEsp(offset);
  default:
    if(offset == 0 && ((regIndex&7)!=5)) return memAddrPtr0(reg);
    if(isByte(offset)) return memAddrPtr1(reg,(char)offset);
    return memAddrPtr4(reg,offset);
  }
}

IntelInstruction &IntelInstruction::memAddrMp2AddReg(const Register &reg, const Register &addReg, BYTE p2, int offset) {
  DEFINEMETHODNAME;
  assert(hasMemAddrMode() && reg.isIndexRegister() && addReg.isIndexRegister());
  const BYTE    regIndex = reg.getIndex(), addRegIndex = addReg.getIndex();
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

IntelInstruction &IntelInstruction::setGPRegImm(const Register &reg, int immv) {
  DEFINEMETHODNAME;
  assert(hasGPRegMode() && isImmMode() && reg.isGPRegister());
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

IntelInstruction &IntelInstruction::prefixSegReg(const Register &reg) {
  static const BYTE segRegPrefix[] = { 0x26,0x2e,0x36,0x3e,0x64,0x65 }; // es,cs,ss,ds,fs,gs
  assert(reg.isSegmentRegister());
  prefix(segRegPrefix[reg.getIndex()]);
  return *this;
}

IntelInstruction &IntelInstruction::setMovGPRegImm(const Register &reg, MovMaxImmType immv) {
  DEFINEMETHODNAME;
  assert(hasGPRegMode() && isImmMode() && reg.isGPRegister());
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
    throwInvalidArgumentException(method,_T("Unknown registersize:%d. reg=%x"), regSize, reg);
    break;
  }
#ifdef IS64BIT
  const BYTE rexbyte = ((regSize==REGSIZE_QWORD)?8:0)|((regIndex>>3)&1);
  SETREXBITS(rexbyte);
#endif
  return *this;
}

IntelInstruction &IntelInstruction::addXMMRegs(const Register &reg1, const Register &reg2) {
  assert(hasXMMRegMode() && reg1.isXMMRegister() && reg2.isXMMRegister());
  setRegSize(reg1);
  const BYTE reg1Index = reg1.getIndex();
  const BYTE reg2Index = reg2.getIndex();

#ifdef IS64BIT
  const BYTE rexbyte = ((reg1Index>>3)&1) | ((reg2Index>>2)&2);
  SETREXBITS(rexbyte);
#endif
  return add(((reg1Index&7)<<3) | (reg2Index&7));
}

#define GPREGISTER(sz,  index) Register(REGTYPE_GP ,sz           ,index) /* General purpose registers */
#define FPUREGISTER(    index) Register(REGTYPE_FPU,REGSIZE_TBYTE,index) /* FPU register              */
#define XMMREGISTER(    index) Register(REGTYPE_XMM,REGSIZE_OWORD,index) /* 128-bit XMM-registers     */
#define SEGMENTREGISTER(index) Register(REGTYPE_SEG,REGSIZE_WORD ,index) /* 16-bit segment registers  */

#pragma warning(disable : 4073)
#pragma init_seg(lib)

const Register AL    = GPREGISTER(REGSIZE_BYTE,  0);
const Register CL    = GPREGISTER(REGSIZE_BYTE,  1);
const Register DL    = GPREGISTER(REGSIZE_BYTE,  2);
const Register BL    = GPREGISTER(REGSIZE_BYTE,  3);
const Register AH    = GPREGISTER(REGSIZE_BYTE,  4);
const Register CH    = GPREGISTER(REGSIZE_BYTE,  5);
const Register DH    = GPREGISTER(REGSIZE_BYTE,  6);
const Register BH    = GPREGISTER(REGSIZE_BYTE,  7);

const Register AX    = GPREGISTER(REGSIZE_WORD,  0);
const Register CX    = GPREGISTER(REGSIZE_WORD,  1);
const Register DX    = GPREGISTER(REGSIZE_WORD,  2);
const Register BX    = GPREGISTER(REGSIZE_WORD,  3);
const Register SP    = GPREGISTER(REGSIZE_WORD,  4);
const Register BP    = GPREGISTER(REGSIZE_WORD,  5);
const Register SI    = GPREGISTER(REGSIZE_WORD,  6);
const Register DI    = GPREGISTER(REGSIZE_WORD,  7);

const Register EAX   = GPREGISTER(REGSIZE_DWORD, 0);
const Register ECX   = GPREGISTER(REGSIZE_DWORD, 1);
const Register EDX   = GPREGISTER(REGSIZE_DWORD, 2);
const Register EBX   = GPREGISTER(REGSIZE_DWORD, 3);
const Register ESP   = GPREGISTER(REGSIZE_DWORD, 4);
const Register EBP   = GPREGISTER(REGSIZE_DWORD, 5);
const Register ESI   = GPREGISTER(REGSIZE_DWORD, 6);
const Register EDI   = GPREGISTER(REGSIZE_DWORD, 7);

#ifdef IS64BIT

// 8 bit registers (only x64);
const Register R8B   = GPREGISTER(REGSIZE_BYTE , 8);
const Register R9B   = GPREGISTER(REGSIZE_BYTE , 9);
const Register R10B  = GPREGISTER(REGSIZE_BYTE ,10);
const Register R11B  = GPREGISTER(REGSIZE_BYTE ,11);
const Register R12B  = GPREGISTER(REGSIZE_BYTE ,12);
const Register R13B  = GPREGISTER(REGSIZE_BYTE ,13);
const Register R14B  = GPREGISTER(REGSIZE_BYTE ,14);
const Register R15B  = GPREGISTER(REGSIZE_BYTE ,15);

// 16 bit registers (only x64);
const Register R8W   = GPREGISTER(REGSIZE_WORD , 8);
const Register R9W   = GPREGISTER(REGSIZE_WORD , 9);
const Register R10W  = GPREGISTER(REGSIZE_WORD ,10);
const Register R11W  = GPREGISTER(REGSIZE_WORD ,11);
const Register R12W  = GPREGISTER(REGSIZE_WORD ,12);
const Register R13W  = GPREGISTER(REGSIZE_WORD ,13);
const Register R14W  = GPREGISTER(REGSIZE_WORD ,14);
const Register R15W  = GPREGISTER(REGSIZE_WORD ,15);

// 32 bit registers (only x64);
const Register R8D   = GPREGISTER(REGSIZE_DWORD, 8);
const Register R9D   = GPREGISTER(REGSIZE_DWORD, 9);
const Register R10D  = GPREGISTER(REGSIZE_DWORD,10);
const Register R11D  = GPREGISTER(REGSIZE_DWORD,11);
const Register R12D  = GPREGISTER(REGSIZE_DWORD,12);
const Register R13D  = GPREGISTER(REGSIZE_DWORD,13);
const Register R14D  = GPREGISTER(REGSIZE_DWORD,14);
const Register R15D  = GPREGISTER(REGSIZE_DWORD,15);

// 64 bit registers (only x64);
const Register RAX   = GPREGISTER(REGSIZE_QWORD, 0);
const Register RCX   = GPREGISTER(REGSIZE_QWORD, 1);
const Register RDX   = GPREGISTER(REGSIZE_QWORD, 2);
const Register RBX   = GPREGISTER(REGSIZE_QWORD, 3);
const Register RSP   = GPREGISTER(REGSIZE_QWORD, 4);
const Register RBP   = GPREGISTER(REGSIZE_QWORD, 5);
const Register RSI   = GPREGISTER(REGSIZE_QWORD, 6);
const Register RDI   = GPREGISTER(REGSIZE_QWORD, 7);
const Register R8    = GPREGISTER(REGSIZE_QWORD, 8);
const Register R9    = GPREGISTER(REGSIZE_QWORD, 9);
const Register R10   = GPREGISTER(REGSIZE_QWORD,10);
const Register R11   = GPREGISTER(REGSIZE_QWORD,11);
const Register R12   = GPREGISTER(REGSIZE_QWORD,12);
const Register R13   = GPREGISTER(REGSIZE_QWORD,13);
const Register R14   = GPREGISTER(REGSIZE_QWORD,14);
const Register R15   = GPREGISTER(REGSIZE_QWORD,15);

#endif // IS64BIT

const Register ST0   = FPUREGISTER( 0);
const Register ST1   = FPUREGISTER( 1);
const Register ST2   = FPUREGISTER( 2);
const Register ST3   = FPUREGISTER( 3);
const Register ST4   = FPUREGISTER( 4);
const Register ST5   = FPUREGISTER( 5);
const Register ST6   = FPUREGISTER( 6);
const Register ST7   = FPUREGISTER( 7);

const Register XMM0  = XMMREGISTER( 0);
const Register XMM1  = XMMREGISTER( 1);
const Register XMM2  = XMMREGISTER( 2);
const Register XMM3  = XMMREGISTER( 3);
const Register XMM4  = XMMREGISTER( 4);
const Register XMM5  = XMMREGISTER( 5);
const Register XMM6  = XMMREGISTER( 6);
const Register XMM7  = XMMREGISTER( 7);

#ifdef IS64BIT
const Register XMM8  = XMMREGISTER( 8);
const Register XMM9  = XMMREGISTER( 9);
const Register XMM10 = XMMREGISTER(10);
const Register XMM11 = XMMREGISTER(11);
const Register XMM12 = XMMREGISTER(12);
const Register XMM13 = XMMREGISTER(13);
const Register XMM14 = XMMREGISTER(14);
const Register XMM15 = XMMREGISTER(15);
#endif // IS64BIT

const Register ES    = SEGMENTREGISTER(0);
const Register CS    = SEGMENTREGISTER(1);
const Register SS    = SEGMENTREGISTER(2);
const Register DS    = SEGMENTREGISTER(3);
const Register FS    = SEGMENTREGISTER(4);
const Register GS    = SEGMENTREGISTER(5);
