#include "pch.h"
#include <ProcessTools.h>

HANDLE getProcessHandle(DWORD processID) {
  if(processID == -1) processID = GetCurrentProcessId();
  HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processID);
  if(hProcess == nullptr) {
    throwLastErrorOnSysCallException(_T("OpenProcess"));
  }
  return hProcess;
}
