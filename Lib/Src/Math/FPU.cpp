#include "pch.h"
#include <Math/FPU.h>

static void throwFPUException(const TCHAR *msg, int code) {
  throw FPUException(msg, code);
}

#define CASE(c) case EXCEPTION_##c: throwFPUException(_T(#c), pExp->ExceptionRecord->ExceptionCode)

void FPUexceptionTranslator(UINT u, EXCEPTION_POINTERS *pExp) {
  switch(pExp->ExceptionRecord->ExceptionCode) {
  CASE(FLT_DENORMAL_OPERAND     );
  CASE(FLT_DIVIDE_BY_ZERO       );
  CASE(FLT_INEXACT_RESULT       );
  CASE(FLT_INVALID_OPERATION    );
  CASE(FLT_OVERFLOW             );
  CASE(FLT_STACK_CHECK          );
  CASE(FLT_UNDERFLOW            );
  default:
    exceptionTranslator(u,pExp);
  }
}

_se_translator_function FPU::setExceptionTranslator(_se_translator_function f) { // static
  return _set_se_translator(f);
}

