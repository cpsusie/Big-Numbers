#include "pch.h"
#include <Math/Int128.h>

ADDIN_API HRESULT WINAPI AddIn__int128(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    _int128 x;
    pHelper->getRealObject(&x, sizeof(x));
    char tmp[150];
    _i128toa(x, tmp, nBase);
    strncpy(pResult, tmp, maxResult);
  } catch(...) {
    strcpy(pResult, "");
  }
  return S_OK;
}

ADDIN_API HRESULT WINAPI AddIn__uint128(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    _uint128 x;
    pHelper->getRealObject(&x, sizeof(x));
    char tmp[150];
    _ui128toa(x, tmp, nBase);
    strncpy(pResult, tmp, maxResult);
  } catch(...) {
    strcpy(pResult, "");
  }
  return S_OK;
}
