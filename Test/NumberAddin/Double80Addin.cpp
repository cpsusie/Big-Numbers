#include "pch.h"
#include <MyUtil.h>
#include <comdef.h>
#include <atlconv.h>
#include <Math/Double80.h>
#include "NumberAddIn.h"

ADDIN_API HRESULT WINAPI AddIn_Double80(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  String tmpStr;
  Double80 d;
  pHelper->getRealObject(&d, sizeof(d));
  tmpStr = d.toString();

  USES_CONVERSION;
  const char *cp = T2A(tmpStr.cstr());
  strncpy(pResult, cp, maxResult);
	return S_OK;
}
