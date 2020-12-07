#include "pch.h"

TCHAR *getErrorText(TCHAR *text, int size, HRESULT hr) {
  TCHAR tmp[1024];
  if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), tmp, ARRAYSIZE(tmp), nullptr) == 0) {
//    if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_FROM_HMODULE, nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), text, size, nullptr) == 0) {
    _stprintf(text,_T("Error %#08X"),hr);
    return text;
  }
  _stprintf(text, _T("Error %d:%s"), hr, tmp);
  return text;
}

String getErrorText(HRESULT hr) {
  TCHAR tmp[2000];
  getErrorText(tmp,ARRAYSIZE(tmp),hr);
  size_t l = _tcsclen(tmp);
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
  if(SUCCEEDED(hResult)) {
    return;
  }
  throwException(_T("%s.\n%s"), fileLineText(fileName, line).cstr(), getErrorText(hResult).cstr());
}

void checkResult(const TCHAR *fileName, int line, BOOL ok) {
  if(ok) return;
  throwException(_T("%s.\nCall returned FALSE"), fileLineText(fileName, line).cstr());
}

void throwLastErrorOnSysCallNameException(const String &name) {
  throwException(_T("%s:%s"), name.cstr(), getLastErrorText().cstr());
}

void throwLastErrorOnSysCallException(const TCHAR *method, const TCHAR *syscall) {
  throwException(_T("%s:%s failed. %s"), method, syscall, getLastErrorText().cstr());
}
