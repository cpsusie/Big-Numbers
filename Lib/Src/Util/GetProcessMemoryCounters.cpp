#include "pch.h"
#include <ProcessTools.h>

PROCESS_MEMORY_COUNTERS getProcessMemoryUsage(DWORD processID) {
  HANDLE                  hProcess = getProcessHandle(processID);
  PROCESS_MEMORY_COUNTERS mc;
  if(GetProcessMemoryInfo(hProcess, &mc, sizeof(mc)) == 0) {
    const DWORD error = GetLastError();
    CloseHandle(hProcess);
    throwException(_T("GetProcessMemoryInfo(%d) failed. %s"), getErrorText(error).cstr());
  }

  CloseHandle(hProcess);
  return mc;
}
