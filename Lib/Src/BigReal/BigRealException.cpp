#include "pch.h"

void throwInvalidToleranceException(const TCHAR *function) {
  throwBigRealInvalidArgumentException(function, _T("f <= 0"));
}

void throwBigRealGetIntegralTypeOverflowException(TCHAR const* const function, const BigReal &x, const String &maxStr) {
  throwBigRealException(_T("%s(%s):Overflow. Max=%s"), function, toString(x).cstr(), maxStr.cstr());
}

void throwBigRealGetIntegralTypeUnderflowException(TCHAR const * const function, const BigReal &x, const String &minStr) {
  throwBigRealException(_T("%s(%s):Underflow. Min=%s"), function, toString(x).cstr(), minStr.cstr());
}

void throwBigRealGetIntegralTypeUndefinedException(TCHAR const * const function, const BigReal &x) {
  throwBigRealInvalidArgumentException(function,_T("x=%s"), toString(x).cstr());
}

void throwNotMutableException(TCHAR const * const function, const DigitPool *pool, BYTE flags) {
  throwBigRealException(_T("%s:BigReal not mutable. (digitPool=%s). flags={%s}")
                       ,function
                       ,pool->getName().cstr()
                       ,BigReal::flagsToString(flags).cstr());
}

void throwMutableBigRealUseConstDigitPoolException(const BigReal &x) {
  throwBigRealException(_T("BigReal uses CONST_DIGITPOOL, which is reserved for ConstBigReal/ConstBigInt")
                       ,x.getDigitPool()->getName().cstr()
                       ,x.flagsToString().cstr());
}

void throwBigRealInvalidArgumentException(const TCHAR *function, _In_z_ _Printf_format_string_ TCHAR const * const Format,...) {
  va_list argptr;
  va_start(argptr, Format);
  const String msg = vformat(Format, argptr);
  va_end(argptr);

  throwBigRealException(_T("%s:Invalid argument:%s"), function, msg.cstr());
}

void throwNotValidException(TCHAR const * const function, _In_z_ _Printf_format_string_ const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format, argptr);
  va_end(argptr);
  throwBigRealException(_T("%s:%s"), function, msg.cstr());
}

void throwBigRealException(_In_z_ _Printf_format_string_ TCHAR const * const Format,...) {
  va_list argptr;
  va_start(argptr, Format);
  const String tmp = vformat(Format, argptr);
  va_end(argptr);
  throw BigRealException(format(_T("BigReal::%s"), tmp.cstr()).cstr());
}
