#pragma once

#include <NatvisAddInHelper.h>

/* Exported Functions */
ADDIN_API HRESULT WINAPI AddIn_BitSet(   DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/);
ADDIN_API HRESULT WINAPI AddIn_Date(     DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/);
ADDIN_API HRESULT WINAPI AddIn_Time(     DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/);
ADDIN_API HRESULT WINAPI AddIn_Timestamp(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/);
