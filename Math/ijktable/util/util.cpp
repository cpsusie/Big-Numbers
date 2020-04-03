#include "stdafx.h"

string vformat(_In_z_ _Printf_format_string_ char const * const format, va_list argptr) {
  char tmp[2000];
  vsprintf_s(tmp, format, argptr);
  return tmp;
}

string format(_In_z_ _Printf_format_string_ char const * const format,...) {
  va_list argptr;

  va_start(argptr, format);
  string s = vformat(format, argptr);
  va_end(argptr);
  return s;
}
