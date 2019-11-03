#include "pch.h"

BigReal inputBigReal(DigitPool &digitPool, _In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  for(;;) {
    va_list argptr;
    va_start(argptr, format);
    _vtprintf(format, argptr);
    va_end(argptr);
    String line;
    readLine(stdin, line);
    line.trim();
    try {
      if(line.equalsIgnoreCase(_T("nan"))) {
        return digitPool.nan();
      } else if(line.equalsIgnoreCase(_T("inf"))) {
        return digitPool.pinf();
      } else if(line.equalsIgnoreCase(_T("-inf"))) {
        return digitPool.ninf();
      }
      BigReal x(line, &digitPool);
      return x;
    } catch(Exception e) {
      // ignore
    }
  }
}
