#include "stdafx.h"

void errorMessage(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr, format);
  String msg = vformat(format, argptr);
  va_end(argptr);
  AfxMessageBox(msg.cstr(), MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
}
