#pragma once

#include <NatvisAddInHelper.h>

/* Exported Functions */
ADDIN_API HRESULT WINAPI AddIn_GameKey(          DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/);
ADDIN_API HRESULT WINAPI AddIn_PositionSignature(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/);
ADDIN_API HRESULT WINAPI AddIn_MoveBase(         DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/);
ADDIN_API HRESULT WINAPI AddIn_EndGameResult(    DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/);
ADDIN_API HRESULT WINAPI AddIn_MoveWithResult(   DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/);
ADDIN_API HRESULT WINAPI AddIn_MoveWithResult2(  DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/);
