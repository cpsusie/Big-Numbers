#include "pch.h"
#include <MyUtil.h>
#include <comdef.h>
#include <atlconv.h>
#include <Math/Int128.h>
#include "NumberAddIn.h"


ADDIN_API HRESULT WINAPI AddIn__int128(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  char tmp[150];
  _int128 x;
  pHelper->getRealObject(&x, sizeof(x));
  _i128toa(x, tmp, 10);
  strncpy(pResult, tmp, maxResult);
	return S_OK;
}

ADDIN_API HRESULT WINAPI AddIn__uint128(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  char tmp[150];
  String tmpStr;
  try {
    _uint128 x;
    pHelper->getRealObject(&x, sizeof(x));
    _ui128toa(x, tmp, 10);
    strncpy(pResult, tmp, maxResult);
    return S_OK;
  } catch (Exception e) {
    tmpStr = format(_T("%s"), e.what());
  }
  catch (...) {
    tmpStr = _T("unknown exception");
  }
  USES_CONVERSION;
  const char *cp = T2A(tmpStr.cstr());
  strncpy(pResult, cp, maxResult);
	return S_OK;
}
