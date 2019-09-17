#pragma once

#include <NatvisAddInHelper.h>

/* Exported Functions */
ADDIN_API HRESULT WINAPI AddIn_BitSet8(         DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/);
ADDIN_API HRESULT WINAPI AddIn_BitSet16(        DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/);
ADDIN_API HRESULT WINAPI AddIn_BitSet32(        DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/);
ADDIN_API HRESULT WINAPI AddIn_BitSet64(        DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/);
ADDIN_API HRESULT WINAPI AddIn_BitSet(          DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/);
ADDIN_API HRESULT WINAPI AddIn_Date(            DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/);
ADDIN_API HRESULT WINAPI AddIn_Time(            DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/);
ADDIN_API HRESULT WINAPI AddIn_Timestamp(       DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/);
ADDIN_API HRESULT WINAPI AddIn_StreamParameters(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/);
