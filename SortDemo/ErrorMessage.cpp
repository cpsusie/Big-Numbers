#include "stdafx.h"

void errorMessage(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr, format);
  vshowMessageBox(MB_APPLMODAL | MB_ICONEXCLAMATION, format, argptr);
  va_end(argptr);
}
