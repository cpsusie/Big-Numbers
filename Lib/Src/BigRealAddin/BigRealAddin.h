#pragma once

#include <NatvisAddInHelper.h>

/* Exported Functions */
ADDIN_API HRESULT WINAPI AddIn_BigReal( DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/);
