#include "pch.h"

double inputDouble(TCHAR *format,...) {
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
