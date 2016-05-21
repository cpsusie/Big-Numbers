#include "pch.h"
#include <Math/MathException.h>

void throwMathException(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  String tmp = vformat(format, argptr);
  va_end(argptr);
  throw MathException(tmp.cstr());
}
