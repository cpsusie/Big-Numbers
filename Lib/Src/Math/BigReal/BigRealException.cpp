#include "pch.h"

void throwInvalidToleranceException(const TCHAR *function) {
  throwBigRealInvalidArgumentException(function, _T("f <= 0"));
}

void throwBigRealGetIntegralTypeOverflowException(const TCHAR *function, const BigReal &x, const String &maxStr) {
  throwBigRealException(_T("%s(%s):Overflow. Max=%s"), function, toString(x).cstr(), maxStr.cstr());
}

void throwBigRealGetIntegralTypeUnderflowException(const TCHAR *function, const BigReal &x, const String &minStr) {
  throwBigRealException(_T("%s(%s):Underflow. Min=%s"), function, toString(x).cstr(), minStr.cstr());
}

void throwBigRealInvalidArgumentException(const TCHAR *function, const TCHAR *Format,...) {
  va_list argptr;
  va_start(argptr, Format);
  const String msg = vformat(Format, argptr);
  va_end(argptr);

  throwBigRealException(_T("%s: Invalid argument:%s"), function, msg.cstr());
}

void throwBigRealException(const TCHAR *Format,...) {
  va_list argptr;
  va_start(argptr, Format);
  const String tmp = vformat(Format, argptr);
  va_end(argptr);
  throw BigRealException(format(_T("BigReal::%s"), tmp.cstr()).cstr());
}

