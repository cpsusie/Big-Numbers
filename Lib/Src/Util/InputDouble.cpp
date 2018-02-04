#include "pch.h"

double inputDouble(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  for(;;) {
    va_list argptr;
    va_start(argptr,format);
    _vtprintf(format, argptr);
    va_end(argptr);
    TCHAR line[100];
    GETS(line);
    double d;
    if(_stscanf(line,_T("%le"),&d) == 1)  {
      return d;
    }
  }
}
