#include "pch.h"
#include <ThreadBase.h>

 // call DuplicateHAndle(GetCurrentThread())
 // returned Handle should be closed with CloseHandle
 // after use. Throw Exception on error
HANDLE getCurrentThreadHandle() {
  HANDLE processHandle = GetCurrentProcess();
  HANDLE threadHandle  = GetCurrentThread();
  HANDLE result;
  if(!DuplicateHandle(processHandle, threadHandle, processHandle, &result, 0, true, DUPLICATE_SAME_ACCESS)) {
    throwLastErrorOnSysCallException(__TFUNCTION__);
  }
  return result;
}
