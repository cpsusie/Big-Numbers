#include "pch.h"
/*
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
*/

int vshowMessageBox(int flags, const TCHAR *format, va_list argptr) {
  return MessageBox(NULL
                   ,vformat(format, argptr).cstr()
                   ,FileNameSplitter(getModuleFileName()).getFileName().cstr()
                   ,flags);
}

void showException(const Exception &e, int flags) {
  showMessageBox(flags, _T("Exception:%s"), e.what());
}
