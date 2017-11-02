#include "pch.h"
#include <Math/Double80.h>

ADDIN_API HRESULT WINAPI AddIn_Double80(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    Double80 d;
    pHelper->getRealObject(&d, sizeof(d));
    char str[150];
    const int expo10 = Double80::getExpo10(d);
    if(expo10 < -4 || (expo10 >= 18)) {
      Double80::d80toa(str, d);
    } else {
      String s = toString(d, 17-expo10,19,ios::fixed);
      USES_CONVERSION;
      strcpy(str,T2A(s.cstr()));
    }
    strncpy(pResult, str, maxResult);
  } catch (...) {
    strncpy(pResult, "", maxResult);
  }
  return S_OK;
}
