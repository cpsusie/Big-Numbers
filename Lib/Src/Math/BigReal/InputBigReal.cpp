#include "pch.h"

BigReal inputBigReal(DigitPool &digitPool, const char *format,...) {
  for(;;) {
    va_list argptr;
    va_start(argptr,format);
    vprintf(format,argptr);
    va_end(argptr);
    char line[4000];
    fgets(line, sizeof(line), stdin);
    try {
      BigReal x(line, &digitPool);
      return x;
    } catch(Exception e) {
      // ignore
    }
  }
}
