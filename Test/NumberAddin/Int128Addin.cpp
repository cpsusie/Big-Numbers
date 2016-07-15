#include "pch.h"
#include <MyUtil.h>
#include "NumberAddIn.h"

ADDIN_API HRESULT WINAPI AddIn__int128(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  char tmp[50];
  sprintf(tmp, " vers:%d base:%d procType:%d", pHelper->dwVersion, nBase, pHelper->GetProcessorType(pHelper));
  strcat(strncpy(pResult, "_int128", maxResult), tmp);
	return S_OK;
}

ADDIN_API HRESULT WINAPI AddIn__uint128(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  char tmp[10];
  sprintf(tmp, " base:%d", nBase);
  strcat(strncpy(pResult, "_uint128", maxResult), tmp);
	return S_OK;
}
