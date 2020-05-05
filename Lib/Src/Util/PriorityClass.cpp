#include "pch.h"
#include <ProcessTools.h>

void setPriorityClass(PriorityClass prClass, HANDLE hProcess) {
  if(!SetPriorityClass(hProcess ? hProcess : GetCurrentProcess(), prClass)) {
    throwLastErrorOnSysCallException(__TFUNCTION__);
  }
}


PriorityClass getPriorityClass(HANDLE hProcess) {
  const DWORD result = GetPriorityClass(hProcess ? hProcess : GetCurrentProcess());
  if(result == 0) {
    throwLastErrorOnSysCallException(__TFUNCTION__);
  }
  return (PriorityClass)result;
}
