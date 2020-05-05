#include "pch.h"
#include <ThreadBase.h>

void setThreadIdealProcessor(DWORD cpu, HANDLE hThread) {
  if(hThread == INVALID_HANDLE_VALUE) hThread = GetCurrentThread();
  if(SetThreadIdealProcessor(hThread,cpu) == (DWORD)-1) {
    throwLastErrorOnSysCallException(__TFUNCTION__);
  }
}
