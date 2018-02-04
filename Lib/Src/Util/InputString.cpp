#include "pch.h"

String inputString(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr,format);
  _vtprintf(format, argptr);
  va_end(argptr);

  String result;
  readLine(stdin, result);
  return result;
}
