#include "pch.h"
#include <Thread.h>

String getThreadDescription(HANDLE hThread) {
  wchar_t *desc;
  CHECKRESULT(GetThreadDescription(hThread, &desc));
  const String result = desc;
  LocalFree(desc);
  return result;
}

void setThreadDescription(HANDLE hThread, const String &description) {
  CHECKRESULT(SetThreadDescription(hThread, description.cstr()));
}
