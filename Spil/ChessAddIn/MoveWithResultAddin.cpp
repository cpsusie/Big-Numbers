#include "pch.h"

ADDIN_API HRESULT WINAPI AddIn_MoveWithResult(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    MoveWithResult mr;
    pHelper->getRealObject(&mr, sizeof(mr));
    const String str = mr.toString();
    USES_CONVERSION;
    const char *cp = T2A(str.cstr());
    strncpy(pResult, cp, maxResult);
  } catch (...) {
    strcpy(pResult, "");
  }
  return S_OK;
}

ADDIN_API HRESULT WINAPI AddIn_MoveWithResult2(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    MoveWithResult2 mr;
    pHelper->getRealObject(&mr, sizeof(mr));
    const String str = mr.toString();
    USES_CONVERSION;
    const char *cp = T2A(str.cstr());
    strncpy(pResult, cp, maxResult);
  } catch (...) {
    strcpy(pResult, "");
  }
  return S_OK;
}

ADDIN_API HRESULT WINAPI AddIn_MoveBase(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    MoveBase mb;
    pHelper->getRealObject(&mb, sizeof(mb));
    const String str = mb.toString();
    USES_CONVERSION;
    const char *cp = T2A(str.cstr());
    strncpy(pResult, cp, maxResult);
  } catch (...) {
    strcpy(pResult, "");
  }
  return S_OK;
}
