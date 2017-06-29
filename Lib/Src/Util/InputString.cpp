#include "pch.h"

String inputString(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  _vtprintf(format, argptr);
  va_end(argptr);

  String result;
  readLine(stdin, result);
  return result;
}
