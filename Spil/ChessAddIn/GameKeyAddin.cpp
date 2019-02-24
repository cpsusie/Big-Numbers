#include "pch.h"

ADDIN_API HRESULT WINAPI AddIn_GameKey(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    GameKey key;
    pHelper->getRealObject(&key, sizeof(key));
    const String str = key.toFENString();
    USES_CONVERSION;
    const char *cp = T2A(str.cstr());
    strncpy(pResult, cp, maxResult);
  } catch (...) {
    strcpy(pResult, "");
  }
  return S_OK;
}
