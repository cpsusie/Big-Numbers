#include "pch.h"
#include <ProcessTools.h>

void setPriorityClass(PriorityClass prClass, HANDLE hProcess) {
  if(!SetPriorityClass(hProcess ? hProcess : GetCurrentProcess(), prClass)) {
    throwLastErrorOnSysCallException(__TFUNCTION__, _T("SetPriorityClass"));
  }
}


PriorityClass getPriorityClass(HANDLE hProcess) {
  const DWORD result = GetPriorityClass(hProcess ? hProcess : GetCurrentProcess());
  if(result == 0) {
    throwLastErrorOnSysCallException(__TFUNCTION__, _T("GetPriorityClass"));
  }
  return (PriorityClass)result;
}
