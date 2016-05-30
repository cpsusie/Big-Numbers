#include "pch.h"
#include <LMCons.h>

String getUserName() {
  TCHAR result[UNLEN+1];
  DWORD size = ARRAYSIZE(result);
  if(GetUserName(result, &size) == 0) {
    throwLastErrorOnSysCallException(_T("GetUserName"));
  }
  return result;
}


