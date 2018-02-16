#include "pch.h"

void throwMathException(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr,format);
  String tmp = vformat(format, argptr);
  va_end(argptr);
  throw MathException(tmp.cstr());
}
