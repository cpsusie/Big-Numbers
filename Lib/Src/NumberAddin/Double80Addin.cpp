#include "pch.h"
#include <Math/Double80.h>

ADDIN_API HRESULT WINAPI AddIn_Double80(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    Double80 d;
    pHelper->getRealObject(&d, sizeof(d));
    char str[150];
    Double80::d80toa(str, d);
    strncpy(pResult, str, maxResult);
  }
  catch (...) {
    strncpy(pResult, "Invalid address", maxResult);
  }
  return S_OK;
}
