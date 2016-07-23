#include "pch.h"
#include <Date.h>

ADDIN_API HRESULT WINAPI AddIn_Date(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    Date d;
    pHelper->getRealObject(&d, sizeof(d));
    TCHAR str[30];
    d.tostr(str);
    USES_CONVERSION;
    const char *cp = T2A(str);
    strncpy(pResult, cp, maxResult);
    return S_OK;
  }
  catch (...) {
    strcpy(pResult, "");
    return S_OK;
  }
}

ADDIN_API HRESULT WINAPI AddIn_Time(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    Time t;
    pHelper->getRealObject(&t, sizeof(t));
    TCHAR str[30];
    t.tostr(str);
    USES_CONVERSION;
    const char *cp = T2A(str);
    strncpy(pResult, cp, maxResult);
    return S_OK;
  }
  catch (...) {
    strcpy(pResult, "");
    return S_OK;
  }
}

ADDIN_API HRESULT WINAPI AddIn_Timestamp(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    Timestamp t;
    pHelper->getRealObject(&t, sizeof(t));
    TCHAR str[50];
    t.tostr(str, ddMMyyyyhhmmssSSS);
    USES_CONVERSION;
    const char *cp = T2A(str);
    strncpy(pResult, cp, maxResult);
    return S_OK;
  }
  catch (...) {
    strcpy(pResult, "");
    return S_OK;
  }
}
