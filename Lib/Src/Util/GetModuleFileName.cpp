#include "pch.h"

String getModuleFileName(HMODULE module) {
  TCHAR result[_MAX_PATH];
  if(GetModuleFileName(module,result,ARRAYSIZE(result)) == 0) {
    throwLastErrorOnSysCallException(__TFUNCTION__, _T("GetModuleFileName"));
  }
  return result;
}


