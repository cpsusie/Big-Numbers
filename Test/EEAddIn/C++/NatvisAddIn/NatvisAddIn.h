// NatvisAddIn.h : main header file for the NatvisAddIn DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#define ADDIN_API __declspec(dllexport)

/* DebugHelper structure used from within the */
typedef struct tagDEBUGHELPER
{
	DWORD dwVersion;
	BOOL (WINAPI *ReadDebuggeeMemory)( struct tagDEBUGHELPER *pThis, DWORD dwAddr, DWORD nWant, VOID* pWhere, DWORD *nGot );
	// from here only when dwVersion >= 0x20000
	unsigned __int64 (WINAPI *GetRealAddress)( struct tagDEBUGHELPER *pThis );
	BOOL (WINAPI *ReadDebuggeeMemoryEx)( struct tagDEBUGHELPER *pThis, unsigned __int64 qwAddr, DWORD nWant, VOID* pWhere, DWORD *nGot );
	int (WINAPI *GetProcessorType)( struct tagDEBUGHELPER *pThis );
} DEBUGHELPER;

/* Exported Functions */
ADDIN_API HRESULT WINAPI AddIn_SystemTime( DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/ );
ADDIN_API HRESULT WINAPI AddIn_FileTime( DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/ );
ADDIN_API HRESULT WINAPI AddIn_COleDateTime( DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/ );
ADDIN_API HRESULT WINAPI AddIn_COleDateTimeSpan( DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/ );
