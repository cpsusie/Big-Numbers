#include "stdafx.h"

void errorMessage(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr, format);
  vshowMessageBox(MB_APPLMODAL | MB_ICONEXCLAMATION, format, argptr);
  va_end(argptr);
}
