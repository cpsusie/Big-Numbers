#include "pch.h"
#include <MyUtil.h>

void pause() {
  pause(_T("Tryk enter..."));
}

void pause(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  TCHAR line[100];
  va_list argptr;
  va_start(argptr,format);
  _vtprintf(format,argptr);
  va_end(argptr);
  GETS(line);
}
