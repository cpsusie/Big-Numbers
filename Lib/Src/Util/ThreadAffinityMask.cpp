#include "pch.h"
#include <ThreadBase.h>

void setThreadAffinityMask(DWORD mask, HANDLE hThread) {
  if(hThread == INVALID_HANDLE_VALUE) hThread = GetCurrentThread();
  if(!SetThreadAffinityMask(hThread, mask)) {
    throwLastErrorOnSysCallException(__TFUNCTION__,_T("SetThreadAffinityMask"));
  }
}
