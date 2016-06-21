#include "pch.h"

BigInt inputBigInt(DigitPool &digitPool, const TCHAR *format,...) {
  for(;;) {
    va_list argptr;
    va_start(argptr, format);
    _vtprintf(format, argptr);
    va_end(argptr);
    char line[10000];
    fgets(line, sizeof(line), stdin);
    try {
      BigInt x(line, &digitPool);
      return x;
    } catch(Exception e) {
      // ignore
    }
  }
}

