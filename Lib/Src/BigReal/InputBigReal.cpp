#include "pch.h"

BigReal inputBigReal(DigitPool &digitPool, _In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  for(;;) {
    va_list argptr;
    va_start(argptr, format);
    _vtprintf(format, argptr);
    va_end(argptr);
    char line[10000];
    fgets(line, sizeof(line), stdin);
    try {
      BigReal x(line, &digitPool);
      return x;
    } catch(Exception e) {
      // ignore
    }
  }
}
