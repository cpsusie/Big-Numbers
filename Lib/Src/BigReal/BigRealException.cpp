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

#if defined(_DEBUG)
void throwNotMutableException(TCHAR const * const file, int line, TCHAR const * const function, const BigReal &x, TCHAR const * const name) {
  throwBigRealException(_T("%s(%d):%s:%s not mutable. digitPool=%s, flags={%s}")
                       ,file, line,function
                       ,name
                       ,x.getDigitPool()->getName().cstr()
                       ,x.flagsToString().cstr());
}
void throwNotMutableException(TCHAR const * const file, int line, TCHAR const * const function, const BigRational &x, TCHAR const * const name) {
  throwBigRealException(_T("%s(%d):%s:%s not mutable. digitPool=%s, flags={%s}")
                       , file, line, function
                       , name
                       , x.getDigitPool()->getName().cstr()
                       , x.flagsToString().cstr());
}
#else
void throwNotMutableException(TCHAR const * const function, const BigReal &x) {
  throwBigRealException(_T("%s:BigReal mutable. digitPool=%s, flags={%s}")
                       , function
                       , x.getDigitPool()->getName().cstr()
                       , x.flagsToString().cstr());
}
void throwNotMutableException(TCHAR const * const function, const BigRational &x) {
  throwBigRealException(_T("%s:BigRational not mutable. digitPool=%s, flags={%s}")
                       , function
                       , x.getDigitPool()->getName().cstr()
                       , x.flagsToString().cstr());
}
#endif // _DEBUG

void throwBigRealInvalidArgumentException(const TCHAR *function, _In_z_ _Printf_format_string_ TCHAR const * const Format,...) {
  va_list argptr;
  va_start(argptr, Format);
  const String msg = vformat(Format, argptr);
  va_end(argptr);

  throwBigRealException(_T("%s:Invalid argument:%s"), function, msg.cstr());
}

void throwNotValidException(TCHAR const * const file, int line, TCHAR const * const name, _In_z_ _Printf_format_string_ const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format, argptr);
  va_end(argptr);
  throwBigRealException(_T("%s(%d):%s:%s"), file, line,name, msg.cstr());
}

void BigReal::throwNotValidException(TCHAR const * const file, int line, TCHAR const * const name, _In_z_ _Printf_format_string_ const TCHAR *Format, ...) const {
  va_list argptr;
  va_start(argptr, Format);
  const String msg = vformat(Format, argptr);
  va_end(argptr);
  String dumpString = format(_T("flags:%s, m_expo:%s, m_low:%s")
                            ,flagsToString().cstr()
                            ,format1000(m_expo).cstr()
                            ,format1000(m_low).cstr());

  throwBigRealException(_T("%s(%d):%s:%s, members:%s"), file, line, name, msg.cstr(), dumpString.cstr());
}

void throwBigRealException(_In_z_ _Printf_format_string_ TCHAR const * const Format,...) {
  va_list argptr;
  va_start(argptr, Format);
  const String tmp = vformat(Format, argptr);
  va_end(argptr);
  throw BigRealException(format(_T("BigReal::%s"), tmp.cstr()).cstr());
}
