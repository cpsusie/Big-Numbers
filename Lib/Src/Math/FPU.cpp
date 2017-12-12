#include "pch.h"
#include <Math/FPU.h>
#include <Math/Double80.h>

#define CASE(c) case EXCEPTION_##c:

void FPUexceptionTranslator(UINT u, EXCEPTION_POINTERS *pExp) {
  switch(pExp->ExceptionRecord->ExceptionCode) {
  CASE(FLT_DENORMAL_OPERAND     );
  CASE(FLT_DIVIDE_BY_ZERO       );
  CASE(FLT_INEXACT_RESULT       );
  CASE(FLT_INVALID_OPERATION    );
  CASE(FLT_OVERFLOW             );
  CASE(FLT_STACK_CHECK          );
  CASE(FLT_UNDERFLOW            );
    throw FPUException(pExp->ExceptionRecord->ExceptionCode);
  default:
    exceptionTranslator(u,pExp);
  }
}

_se_translator_function FPU::setExceptionTranslator(_se_translator_function f) { // static
  return _set_se_translator(f);
}

FPUControlWord FPU::adjustExceptionMask(USHORT enable, USHORT disable) { // static
  FPUControlWord cw = getControlWord(), old = cw;
  enable  &= FPU_ALL_EXCEPTIONS;  // We are only interested in the first 6 bits
  disable &= FPU_ALL_EXCEPTIONS;
  cw.m_data &= ~enable; // 0-bit ENABLES the interrupt, a 1-bit DISABLES it
  cw.m_data |= disable;
  setControlWord(cw);
  return old;
}


String FPUControlWord::toString() const {
  static const TCHAR *precisionStr[] = {
    _T("real4")
   ,_T("unused")
   ,_T("real8")
   ,_T("real10")
  };
  static const TCHAR *roundStr[] = {
    _T("nearest")
   ,_T("down")
   ,_T("up")
   ,_T("truncate")
  };
  const TCHAR *delim = EMPTYSTRING;
  String result,interruptStr;

#define ADDINTERRUPTFIELD(name) if(!m_##name) { interruptStr += delim; interruptStr += _T(#name); delim = _T(","); }

  ADDINTERRUPTFIELD(im);
  ADDINTERRUPTFIELD(dm);
  ADDINTERRUPTFIELD(zm);
  ADDINTERRUPTFIELD(om);
  ADDINTERRUPTFIELD(um);
  ADDINTERRUPTFIELD(pm);
  ADDINTERRUPTFIELD(iem);
  return format(_T("ControlWord:InterruptMask:{%s}, Precision:%s, Round:%s")
               ,interruptStr.cstr()
               ,precisionStr[m_pc]
               ,roundStr[m_rc]
               );
}

String FPUStatusWord::toString() const {
  const TCHAR *delim = EMPTYSTRING;
  String result,exceptionStr;

#define ADDEXCEPTIONFIELD(name) if(m_##name) { exceptionStr += delim; exceptionStr += _T("e_" #name); delim = _T(","); }

  ADDEXCEPTIONFIELD(i );
  ADDEXCEPTIONFIELD(d );
  ADDEXCEPTIONFIELD(z );
  ADDEXCEPTIONFIELD(o );
  ADDEXCEPTIONFIELD(u );
  ADDEXCEPTIONFIELD(p );
  ADDEXCEPTIONFIELD(sf);
  return format(_T("StatusWord:Exceptions:{%s}, IR:%d, C[0-3]:[%d%d%d%d], Stacktop:%d, Busy:%d")
               ,exceptionStr.cstr()
               ,m_ir
               ,m_c0,m_c1,m_c2,m_c3
               ,m_top
               ,m_busy
               );
}

const TCHAR *FPUTagWord::getContentStr(FPURegisterContent content) { // static
  static const TCHAR *contentStr[] = {
    _T("nz")
   ,_T("z")
   ,_T("nan")
   ,_T("---")
  };
  return contentStr[content];
}

String FPUTagWord::toString() const {
  String tmp;
  const TCHAR *delim = NULL;
  for(int i = 0; i < 8; i++) {
    if(delim) tmp += delim; else delim = _T(",");
    tmp += getContentStr(getContent(i));
  }
  return format(_T("TagWord:%s"), tmp.cstr());
}

String FPUEnvironment::toString() const {
  return format(_T("%s\n%s\n%s\nIP:%08x Op:%08x")
               ,m_ctrlWord.toString().cstr()
               ,m_statusWord.toString().cstr()
               ,m_tagWord.toString().cstr()
               ,m_IP
               ,m_opAddr
               );
}

String FPUState::toString() const {
  String tmp;
  TCHAR  d80str[50];
  for(UINT i = 0; i < 8; i++) {
    switch(m_tagWord.getContent(i)) {
    case FPUREG_VALID_NZ: tmp += format(_T("st%d:%s\n" ), i, d80tot(d80str, getReg(i))); break;
    case FPUREG_ZERO    : tmp += format(_T("st%d:0\n"  ), i); break;
    case FPUREG_NAN     : tmp += format(_T("st%d:Nan\n"), i); break;
    case FPUREG_EMPTY   : tmp += format(_T("st%d:---\n"), i); break;
    }
  }
  return tmp + __super::toString();
}
