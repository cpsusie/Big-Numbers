#include "pch.h"
#include <MyUtil.h>
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
  _uint128 x;
  pHelper->getRealObject(&x, sizeof(x));
  _ui128toa(x, tmp, 10);
  strncpy(pResult, tmp, maxResult);

	return S_OK;
}
