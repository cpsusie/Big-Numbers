#include "pch.h"

void pause() {
  pause(_T("Tryk enter..."));
}

void pause(const TCHAR *format,...) {
  TCHAR line[100];
  va_list argptr;
  va_start(argptr,format);
  _vtprintf(format,argptr);
  va_end(argptr);
  GETS(line);
}
