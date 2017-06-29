#include "pch.h"
#include <conio.h>

String inputPassword(const TCHAR *format,...) {
  TCHAR buffer[256];
  va_list argptr;
  va_start(argptr,format);
  _vtprintf(format, argptr);
  va_end(argptr);

  int i = 0;
  for(;;) {
    const int ch = _getch();
    if(ch == _T('\b')) {
      if(i > 0) {
        i--;
      }
    } else if(ch == _T('\r')) {
      break;
    } else {
      buffer[i++] = ch;
      if(i == ARRAYSIZE(buffer) - 1) {
        break;
      }
    }
  }
  buffer[i] = 0;
  return buffer;
}
