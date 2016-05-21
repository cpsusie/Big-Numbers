#include "pch.h"
#include <ProcessTools.h>

#pragma comment(lib, "advapi32.lib")

CompactArray<DWORD> getProcessIds() {
  DWORD processArray[8000];
  DWORD dwSize;

  if(!EnumProcesses(processArray, sizeof(processArray), &dwSize)) {
    throwLastErrorOnSysCallException(_T("EnumProcesses"));
  }
  const int count = dwSize / sizeof(DWORD);
  CompactArray<DWORD> result;
  for(int i = 0; i < count; i++) {
    result.add(processArray[i]);
  }
  return result;
}

