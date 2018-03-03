#include "pch.h"
#include <Math/Expression/OpCode.h>

void IntelOpcode::throwRegSizeMismatch(BYTE reg) {
  throwInvalidArgumentException(__TFUNCTION__
                               ,_T("register %s doesn't match other operand %s")
                               ,getRegisterName(reg).cstr(), getOpSizeName());
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

IntelOpcode &IntelOpcode::addReg(BYTE reg) {
  assert(m_regMode);
  setOpSize(reg);
  const BYTE regIndex = REGINDEX(reg), regSize  = REGSIZE( reg);
  assert(regIndex <= MAX_REFERENCE_REGISTER);
#ifdef IS64BIT
  const BYTE rexbyte = ((regSize==REGSIZE_QWORD)?8:0)|((regIndex>>1)&4);
  SETREXBITS(rexbyte);
#endif
  switch(regSize) {
  case REGSIZE_BYTE : break;
  case REGSIZE_WORD : wordOp();
    // continue case
  default           : or(1);
  }
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

const TCHAR *IntelOpcode::getOpSizeName(int regSize) { // static
  static const TCHAR *name[] = { _T("BYTE"), _T("WORD"), _T("DWORD"), _T("QWORD") };
  return name[regSize];
}
const TCHAR *IntelOpcode::getOpSizeName() const {
  return m_opSizeDefined ? getOpSizeName(getOpSize()) : _T("Undefined");
}

String IntelOpcode::getRegisterName(BYTE reg) { // static
  const int index   = REGINDEX(reg);
  const int regSize = REGSIZE( reg);
  switch (regSize) {
  case REGSIZE_BYTE :
    switch (index) {
    case  0: return _T("AL"  );
    case  1: return _T("CL"  );
    case  2: return _T("DL"  );
    case  3: return _T("BL"  );
    case  4: return _T("AH"  );
    case  5: return _T("CH"  );
    case  6: return _T("DH"  );
    case  7: return _T("BH"  );
#ifdef IS64BIT
    case  8: return _T("R8B" );
    case  9: return _T("R9B" );
    case 10: return _T("R10B");
    case 11: return _T("R11B");
    case 12: return _T("R12B");
    case 13: return _T("R13B");
    case 14: return _T("R14B");
    case 15: return _T("R15B");
#endif // IS64BIT
    }
    break;
  case REGSIZE_WORD :
    switch (index) {
    case  0: return _T("AX"  );
    case  1: return _T("CX"  );
    case  2: return _T("DX"  );
    case  3: return _T("BX"  );
    case  4: return _T("SP"  );
    case  5: return _T("BP"  );
    case  6: return _T("SI"  );
    case  7: return _T("DI"  );
#ifdef IS64BIT
    case  8: return _T("R8W" );
    case  9: return _T("R9W" );
    case 10: return _T("R10W");
    case 11: return _T("R11W");
    case 12: return _T("R12W");
    case 13: return _T("R13W");
    case 14: return _T("R14W");
    case 15: return _T("R15W");
#endif // IS64BIT
    }
    break;
  case REGSIZE_DWORD:
    switch (index) {
    case  0: return _T("EAX" );
    case  1: return _T("ECX" );
    case  2: return _T("EDX" );
    case  3: return _T("EBX" );
    case  4: return _T("ESP" );
    case  5: return _T("EBP" );
    case  6: return _T("ESI" );
    case  7: return _T("EDI" );
#ifdef IS64BIT
    case  8: return _T("R8D" );
    case  9: return _T("R9D" );
    case 10: return _T("R10D");
    case 11: return _T("R11D");
    case 12: return _T("R12D");
    case 13: return _T("R13D");
    case 14: return _T("R14D");
    case 15: return _T("R15D");
#endif // IS64BIT
    }
    break;
#ifdef IS64BIT
  case REGSIZE_QWORD:
    switch (index) {
    case  0: return _T("RAX" );
    case  1: return _T("RCX" );
    case  2: return _T("RDX" );
    case  3: return _T("RBX" );
    case  4: return _T("RSP" );
    case  5: return _T("RBP" );
    case  6: return _T("RSI" );
    case  7: return _T("RDI" );
    case  8: return _T("R8"  );
    case  9: return _T("R9"  );
    case 10: return _T("R10" );
    case 11: return _T("R11" );
    case 12: return _T("R12" );
    case 13: return _T("R13" );
    case 14: return _T("R14" );
    case 15: return _T("R15" );
    }
    break;
#endif // IS64BIT
  }
  return format(_T("Unknown register-value:%d"), reg);
}

IntelInstruction &IntelInstruction::add(INT64 bytesToAdd, BYTE count) {
  assert(m_size+count < MAX_INSTRUCTIONSIZE);
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
IntelInstruction &IntelInstruction::setReg(BYTE reg) {
  assert(m_regMode);
  setOpSize(reg);
  const BYTE regIndex = REGINDEX(reg), regSize  = REGSIZE( reg);
  assert(regIndex <= MAX_REFERENCE_REGISTER);
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
  or(regIndex&7);
  return *this;
}

IntelInstruction &IntelInstruction::setRegImm(BYTE reg, int immv) {
  DEFINEMETHODNAME;
  const BYTE regIndex = REGINDEX(reg), regSize  = REGSIZE(reg);
  assert(m_regMode && isImmMode());
  assert(regIndex <= MAX_REFERENCE_REGISTER);
  setOpSize(reg);
  switch(regSize) {
  case REGSIZE_BYTE :
    if(!isByte(immv)) {
      throwInvalidArgumentException(method,_T("immediate value %08x doesn't fit in %s"),immv, getRegisterName(reg).cstr());
    }
    if(regIndex != 0) {
      or(regIndex&7).prefix(0x80);
    }
    add(immv,1);
    break;
  case REGSIZE_WORD :
    if(!isWord(immv)) {
      throwInvalidArgumentException(method,_T("immediate value %08x doesn't fit in %s"),immv, getRegisterName(reg).cstr());
    }
    if(regIndex != 0) {
      or(regIndex&7).prefix(isByte(immv)?0x83:0x81);
      add(immv,isByte(immv)?1:2);
    } else {
      or(1);
      add(immv,2);
    }
    wordOp();
    break;
  default           :
    if(regIndex != 0) {
      or(regIndex&7).prefix(isByte(immv)?0x83:0x81);
      add(immv,isByte(immv)?1:4);
    } else {
      or(1);
      add(immv,4);
    }
    break;
  }
#ifdef IS64BIT
  const BYTE rexbyte = ((regSize==REGSIZE_QWORD)?8:0)|((regIndex>>3)&1);
  SETREXBITS(rexbyte);
#endif
  return *this;
}
