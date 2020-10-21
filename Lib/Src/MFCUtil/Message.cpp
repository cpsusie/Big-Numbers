#include "pch.h"
#include <MyUtil.h>
#include <FileNameSplitter.h>

/*
void Message(_In_z_ _Printf_format_string_ TCHAR const * const format,... ) {
  va_list argptr;
  va_start(argptr,format);
  const String msg = vformat(format, argptr);
  va_end(argptr);

  try {
    AfxMessageBox(msg.cstr());
  } catch(...) {
    MessageBox(nullptr, msg.cstr(), _T("Error"), MB_ICONASTERISK);
  }
}
*/

int vshowMessageBox(int flags, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  String msg = vformat(format, argptr)
              .replace(_T('\r'),EMPTYSTRING)
              .replace(_T('\n'), _T("\n\r"));
  return MessageBox(nullptr
                   ,msg.cstr()
                   ,FileNameSplitter(getModuleFileName()).getFileName().cstr()
                   ,flags);
}

void showException(const Exception &e, int flags) {
  showMessageBox(flags, _T("Exception:%s"), e.what());
}
