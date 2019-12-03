#include "pch.h"
#include <Thread.h>

String getThreadDescription(HANDLE hThread) {
  wchar_t *desc;
  if(hThread == INVALID_HANDLE_VALUE) hThread = getCurrentThreadHandle();
  CHECKRESULT(GetThreadDescription(hThread, &desc));
  const String result = desc;
  LocalFree(desc);
  return result;
}

void setThreadDescription(const String &description, HANDLE hThread) {
  if(hThread == INVALID_HANDLE_VALUE) hThread = getCurrentThreadHandle();
  CHECKRESULT(SetThreadDescription(hThread, description.cstr()));
}
