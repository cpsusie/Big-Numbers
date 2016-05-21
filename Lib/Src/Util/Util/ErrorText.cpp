#include "pch.h"

TCHAR *getErrorText(TCHAR *text, int size, HRESULT hr) {
  TCHAR tmp[1024];
  if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), tmp, ARRAYSIZE(tmp), NULL) == 0) {
//    if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_FROM_HMODULE, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), text, size, NULL) == 0) {
    _stprintf(text,_T("Error %d"),hr);
    return text;
  }
  _stprintf(text, _T("Error %d:%s"), hr, tmp);
  return text;
}

String getErrorText(HRESULT hr) {
  TCHAR tmp[2000];
  getErrorText(tmp,ARRAYSIZE(tmp),hr);
  int l = _tcsclen(tmp);
  if(l >= 1 && tmp[l-1] == 10) {
    tmp[l-1] = 0;
    if(l >= 2 && tmp[l-2] == 13) {
      tmp[l-2] = 0;
    }
  }
  return tmp;
}

String getLastErrorText() {
  return getErrorText(GetLastError());
}

static String fileLineText(const TCHAR *fileName, int line) {
  return format(_T("Error in %s line %d"), fileName, line);
}

void checkResult(const TCHAR *fileName, int line, ULONG hResult) {
  if(hResult == 0) {
    return;
  }
  throwException(_T("%s.\n%s"), fileLineText(fileName, line).cstr(), getErrorText(hResult).cstr());
}

void checkResult(const TCHAR *fileName, int line, HRESULT hResult) {
  if(hResult == 0) {
    return;
  }
  throwException(_T("%s.\n%s"), fileLineText(fileName, line).cstr(), getErrorText(hResult).cstr());
}

void checkResult(const TCHAR *fileName, int line, BOOL ok) {
  if(ok == TRUE) {
    return;
  }
  throwException(_T("%s.\nCall returned FALSE"), fileLineText(fileName, line).cstr());
}

void throwLastErrorOnSysCallException(const TCHAR *function) {
  throwException(_T("%s failed. %s"), function, getLastErrorText().cstr());
}

void throwMethodLastErrorOnSysCallException(const TCHAR *className, const TCHAR *method) {
  throwException(_T("%s::%s failed. %s"), className, method, getLastErrorText().cstr());
}
