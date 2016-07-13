#include "pch.h"
#include "NumbersAddIn.h"

ADDIN_API HRESULT WINAPI AddIn_BigReal(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
	DWORD nGot;

	// read system time from debuggee memory space
//	HRESULT hr = pHelper->ReadDebuggeeMemoryEx(pHelper, pHelper->GetRealAddress(pHelper), sizeof(SysTime), &SysTime, &nGot);
  strncpy(pResult, "some BigReal", maxResult);
	return S_OK;
}

ADDIN_API HRESULT WINAPI AddIn__int128(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  strncpy(pResult, "Some signed _int128", maxResult);
	return 1;
}

ADDIN_API HRESULT WINAPI AddIn__uint128(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  strncpy(pResult, "Some unsigned _uint128", maxResult);
	return S_OK;
}
