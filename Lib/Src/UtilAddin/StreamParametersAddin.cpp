#include "pch.h"
#include <StreamParameters.h>

ADDIN_API HRESULT WINAPI AddIn_StreamParameters(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    StreamParameters sp;
    pHelper->getRealObject(&sp, sizeof(sp));
    String str = sp.toString();
    char tmp[2000];
    strncpy(pResult, strCpy(tmp, str.cstr()), maxResult);
    return S_OK;
  } catch(...) {
    strcpy(pResult, "");
    return S_OK;
  }
}
