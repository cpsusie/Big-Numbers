#include "pch.h"
#include <Math/Int128.h>

static const char *zeroStr = "00000000000000000000000000000000";

ADDIN_API HRESULT WINAPI AddIn__int128(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    _int128 x;
    pHelper->getRealObject(&x, sizeof(x));
    char tmp[150];
    if(nBase == 16) {
      if(x.isZero()) {
        strcpy(tmp, zeroStr);
      } else {
        _i128toa(x, tmp, nBase);
      }
      strncat(strcpy(pResult, "0x"), tmp, maxResult);
    } else {
      strncpy(pResult, _i128toa(x, tmp, nBase), maxResult);
    }
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
    if(nBase == 16) {
      if(x.isZero()) {
        strcpy(tmp, zeroStr);
      } else {
        _ui128toa(x, tmp, nBase);
      }
      strncat(strcpy(pResult, "0x"), tmp, maxResult);
    } else {
      strncpy(pResult, _ui128toa(x, tmp, nBase), maxResult);
    }
  } catch(...) {
    strcpy(pResult, "");
  }
  return S_OK;
}
