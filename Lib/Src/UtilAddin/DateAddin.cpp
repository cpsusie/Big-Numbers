#include "pch.h"
#include <Date.h>

ADDIN_API HRESULT WINAPI AddIn_Date(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    Date d;
    pHelper->getRealObject(&d, sizeof(d));
    char str[30];
    strncpy(pResult, d.tostr(str), maxResult);
    return S_OK;
  } catch(...) {
    strcpy(pResult, "");
    return S_OK;
  }
}

ADDIN_API HRESULT WINAPI AddIn_Time(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    Time t;
    pHelper->getRealObject(&t, sizeof(t));
    const int sec  = t.getSecond(), msec = t.getMilliSecond();
    char str[50];
    const TCHAR *format = msec ? hhmmssSSS : sec ? hhmmss : hhmm;
    strncpy(pResult, t.tostr(str, format), maxResult);
    return S_OK;
  } catch(...) {
    strcpy(pResult, "");
    return S_OK;
  }
}

ADDIN_API HRESULT WINAPI AddIn_Timestamp(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    Timestamp t;
    pHelper->getRealObject(&t, sizeof(t));
    char str[100];
    strncpy(pResult, t.tostr(str, ddMMyyyyhhmmssSSS), maxResult);
    return S_OK;
  } catch(...) {
    strcpy(pResult, "");
    return S_OK;
  }
}
