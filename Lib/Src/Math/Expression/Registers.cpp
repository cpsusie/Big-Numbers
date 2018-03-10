#include "pch.h"
#include <Math/Expression/Registers.h>

const TCHAR *getOpSizeName(RegSize regSize) {
  static const TCHAR *name[] = { _T("BYTE"), _T("WORD"), _T("DWORD"), _T("QWORD"), _T("TBYTE"), _T("OWORD") };
  return name[regSize];
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

String FPURegister::getName() const {
  return (getIndex() <= 7) ? format(_T("ST%d"), getIndex()) : __super::getName();
}

String XMMRegister::getName() const {
  return (getIndex() <= MAX_XMMREGISTER_INDEX) ? format(_T("XMM%d"), getIndex())
                                                : __super::getName();
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
