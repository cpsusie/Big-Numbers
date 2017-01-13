#include "stdafx.h"
#include <MFCUtil/WinTools.h>

void Message(const TCHAR *format,... ) {
  va_list argptr;
  va_start(argptr,format);
  const String msg = vformat(format, argptr);
  va_end(argptr);

  try {
    AfxMessageBox(msg.cstr());
  } catch(...) {
    MessageBox(NULL, msg.cstr(), _T("Error"), MB_ICONASTERISK);
  }
}

