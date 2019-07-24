#include "pch.h"
#include <MyUtil.h>

UINT inputUint(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  for(;;) {
    va_list argptr;
    va_start(argptr, format);
    _vtprintf(format, argptr);
    va_end(argptr);
    TCHAR line[100];
    GETS(line);
    UINT d;
    if(_stscanf(line,_T("%lu"),&d) == 1) {
      return d;
    }
  }
}
