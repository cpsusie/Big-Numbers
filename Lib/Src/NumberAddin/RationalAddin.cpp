#include "pch.h"
#include <Math/Rational.h>

ADDIN_API HRESULT WINAPI AddIn_Rational(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    Rational r;
    pHelper->getRealObject(&r, sizeof(r));
    char str[150];
    rattoa(str, r, nBase);
    strncpy(pResult, str, maxResult);
  } catch (...) {
    strncpy(pResult, "", maxResult);
  }
  return S_OK;
}
