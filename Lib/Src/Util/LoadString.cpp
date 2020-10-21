#include "pch.h"
#include <MyUtil.h>

String loadString(int id) {
  TCHAR buffer[4096];
  if(LoadString(GetModuleHandle(nullptr), id, buffer, ARRAYSIZE(buffer)) == 0) {
    return format(_T("Unknown resource id:%d:%s"), id, getLastErrorText().cstr());
  }
  LASTVALUE(buffer) = 0;
  return buffer;
};

String loadString(int id, const String &defaultValue) {
  TCHAR buffer[4096];
  if(LoadString(GetModuleHandle(nullptr), id, buffer, ARRAYSIZE(buffer)) == 0) {
    _tcsncpy(buffer, defaultValue.cstr(), ARRAYSIZE(buffer));
  }
  LASTVALUE(buffer) = 0;
  return buffer;
}
