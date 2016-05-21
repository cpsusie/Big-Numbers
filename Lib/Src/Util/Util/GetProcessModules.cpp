#include "pch.h"
#include <ProcessTools.h>

#pragma comment(lib, "advapi32.lib")

CompactArray<HMODULE> getModules(HANDLE hProcess) {
  HMODULE moduleArray[8000];
  DWORD   dwSize;

  if(EnumProcessModules(hProcess, moduleArray, sizeof(moduleArray), &dwSize) == 0) {
    throwLastErrorOnSysCallException(_T("EnumProcessModules"));
  }
  const int count = dwSize / sizeof(HMODULE);
  CompactArray<HMODULE> result;
  for(int i = 0; i < count; i++ ) {
    result.add(moduleArray[i]);
  }
  return result;
}

