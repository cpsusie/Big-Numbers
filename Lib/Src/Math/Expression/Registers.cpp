#include "pch.h"
#include <Math/Expression/Registers.h>

String toString(RegType regType) {
  switch(regType) {
  case REGTYPE_NONE: return _T("NONE");
  case REGTYPE_GPR : return _T("GPR" );
  case REGTYPE_SEG : return _T("SEG" );
  case REGTYPE_FPU : return _T("FPU" );
  case REGTYPE_XMM : return _T("XMM" );
  default          : return format(_T("Unknown register type:%d"), regType);
  }
}

String toString(RegSize regSize) {
  switch(regSize) {
  case REGSIZE_BYTE  : return _T("byte" );
  case REGSIZE_WORD  : return _T("word" );
  case REGSIZE_DWORD : return _T("dword");
  case REGSIZE_QWORD : return _T("qword");
  case REGSIZE_TBYTE : return _T("tbyte");
  case REGSIZE_OWORD : return _T("oword");
  default            : return format(_T("Unknown register size:%d"), regSize);
  }
}

// ---------------------------------- Register ----------------------------

String GPRegister::getName() const {
  switch(getSize()) {
  case REGSIZE_BYTE :
    switch(getIndex()) {
    case  0: return _T("al"  );
    case  1: return _T("cl"  );
    case  2: return _T("dl"  );
    case  3: return _T("bl"  );
    case  4: return _T("ah"  );
    case  5: return _T("ch"  );
    case  6: return _T("dh"  );
    case  7: return _T("bh"  );
#ifdef IS64BIT
    case  8:
    case  9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
      return format(_T("r%db"),getIndex());
#endif // IS64BIT
    }
    break;
  case REGSIZE_WORD :
    switch(getIndex()) {
    case  0: return _T("ax"  );
    case  1: return _T("cx"  );
    case  2: return _T("dx"  );
    case  3: return _T("bx"  );
    case  4: return _T("sp"  );
    case  5: return _T("bp"  );
    case  6: return _T("si"  );
    case  7: return _T("di"  );
#ifdef IS64BIT
    case  8:
    case  9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
      return format(_T("r%dw"),getIndex());
#endif // IS64BIT
    }
    break;
  case REGSIZE_DWORD:
    switch(getIndex()) {
    case  0: return _T("eax" );
    case  1: return _T("ecx" );
    case  2: return _T("edx" );
    case  3: return _T("ebx" );
    case  4: return _T("esp" );
    case  5: return _T("ebp" );
    case  6: return _T("esi" );
    case  7: return _T("edi" );
#ifdef IS64BIT
    case  8:
    case  9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
      return format(_T("r%dd"), getIndex());
#endif // IS64BIT
    }
    break;
#ifdef IS64BIT
  case REGSIZE_QWORD:
    switch(getIndex()) {
    case  0: return _T("rax" );
    case  1: return _T("rcx" );
    case  2: return _T("rdx" );
    case  3: return _T("rbx" );
    case  4: return _T("rsp" );
    case  5: return _T("rbp" );
    case  6: return _T("rsi" );
    case  7: return _T("rdi" );
    case  8:
    case  9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
      return format(_T("r%d"),getIndex());
    }
    break;
#endif // IS64BIT
  } // switch(regSize)
  return __super::getName();
}

String FPURegister::getName() const {
  return (getIndex() <= 7) ? format(_T("st%d"), getIndex()) : __super::getName();
}

String XMMRegister::getName() const {
  return (getIndex() <= MAX_XMMREGISTER_INDEX) ? format(_T("xmm%d"), getIndex())
                                                : __super::getName();
}

String SegmentRegister::getName() const {
  switch(getIndex()) {
  case 0 : return _T("es");
  case 1 : return _T("cs");
  case 2 : return _T("ss");
  case 3 : return _T("ds");
  case 4 : return _T("fs");
  case 5 : return _T("gs");
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
