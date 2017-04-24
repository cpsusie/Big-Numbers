#include "stdafx.h"

#ifndef TABLEBASE_BUILDER

#include "ExternEngine.h"

void ExternInputThread::vverbose(const TCHAR *format, va_list argptr) {
  const String tmp = vformat(format, argptr);
  ::verbose(_T("%s.\n"), tmp.cstr());
}

String ExternInputThread::getLine(int timeoutInMilliseconds) {
  return InputThread::getLine(timeoutInMilliseconds).trim();
}

#endif // TABLEBASE_BUILDER
