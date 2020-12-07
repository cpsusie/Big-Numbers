#include "pch.h"
#include <ThreadBase.h>

void setThreadPriorityBoost(bool disablePriorityBoost, HANDLE hThread) {
  if(hThread == INVALID_HANDLE_VALUE) hThread = GetCurrentThread();
  if(!SetThreadPriorityBoost(hThread, disablePriorityBoost)) {
    throwLastErrorOnSysCallException(__TFUNCTION__, _T("SetThreadPriorityBoost"));
  }
}

bool getThreadPriorityBoost(HANDLE hThread) {
  if(hThread == INVALID_HANDLE_VALUE) hThread = GetCurrentThread();
  BOOL boostDisabled;
  if(GetThreadPriorityBoost(hThread, &boostDisabled)) {
    throwLastErrorOnSysCallException(__TFUNCTION__, _T("GetThreadPriorityBoost"));
  }
  return boostDisabled ? true : false;
}
