#include "pch.h"
#include <ThreadBase.h>

void setThreadPriority(ThreadPriority priority, HANDLE hThread) {
  if(hThread == INVALID_HANDLE_VALUE) hThread = GetCurrentThread();
  if(!SetThreadPriority(hThread, (int)priority)) {
    throwLastErrorOnSysCallException(__TFUNCTION__);
  }
}

// if hThread is not specified, currentThreadHandle is used
ThreadPriority getThreadPriority(HANDLE hThread) {
  if(hThread == INVALID_HANDLE_VALUE) hThread = GetCurrentThread();
  const int result = GetThreadPriority(hThread);
  if(result == THREAD_PRIORITY_ERROR_RETURN) {
    throwLastErrorOnSysCallException(__TFUNCTION__);
  }
  return (ThreadPriority)result;
}
