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

ADDIN_API HRESULT WINAPI AddIn_PositionSignature(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    PositionSignature signature;
    pHelper->getRealObject(&signature, sizeof(signature));
    const String str = signature.toString();
    USES_CONVERSION;
    const char *cp = T2A(str.cstr());
    strncpy(pResult, cp, maxResult);
  } catch (...) {
    strcpy(pResult, "");
  }
  return S_OK;
}
