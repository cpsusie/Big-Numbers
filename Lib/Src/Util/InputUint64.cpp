#include "pch.h"

UINT64 inputUint64(const TCHAR *format,...) {
  for(;;) {
    va_list argptr;
    va_start(argptr,format);
    _vtprintf(format, argptr);
    va_end(argptr);
    TCHAR line[100];
    GETS(line);
    UINT64 d;
    if(_stscanf(line,_T("%llu"),&d) == 1) {
      return d;
    }
  }
}
