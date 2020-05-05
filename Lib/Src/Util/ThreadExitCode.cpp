#include "pch.h"
#include <ThreadBase.h>

ULONG getThreadExitCode(HANDLE hThread) {
  if(hThread == INVALID_HANDLE_VALUE) hThread = GetCurrentThread();
  DWORD exitCode = 0;
  if(!GetExitCodeThread(hThread,&exitCode)) {
    throwLastErrorOnSysCallException(__TFUNCTION__);
  }
  return exitCode;
}
