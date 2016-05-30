#include "pch.h"
#include <objbase.h>

GUID *newGUID(GUID *guid) {
  HRESULT hr = CoCreateGuid(guid);
  if(hr != S_OK) {
    throwException(_T("%s"),getErrorText(hr).cstr());
  }
  return guid;
}

TCHAR *sprintGUID(TCHAR *dst, const GUID &guid) {
  short w4 = ((short)guid.Data4[0] << 8) | guid.Data4[1];
  short w5 = ((short)guid.Data4[2] << 8) | guid.Data4[3];
  short w6 = ((short)guid.Data4[4] << 8) | guid.Data4[5];
  short w7 = ((short)guid.Data4[6] << 8) | guid.Data4[7];
  _stprintf(dst,_T("%08X-%04hX-%04hX-%04hX-%04hX%04hX%04hX")
             ,guid.Data1,guid.Data2,guid.Data3,w4,w5,w6,w7);
  return dst;
}

TCHAR *newGUID(TCHAR *dst) {
  GUID guid;
  return sprintGUID(dst,*newGUID(&guid));
}
