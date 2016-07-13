// NatvisAddIn.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "NatvisAddIn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//Helper Functions_______________________________________________________________________________________________________________
static HRESULT FormatDateTime( SYSTEMTIME *pSystemTime, char *pResult, size_t maxResult )
{
	GetDateFormat( GetThreadLocale(), DATE_SHORTDATE, pSystemTime, NULL, pResult, (int)maxResult );
	size_t len = _tcslen( pResult );
	if ( (maxResult - len) < 2 )
		return E_FAIL;      // if not enough room in buffer
	pResult[ len ] = ' ';
	len++;
	GetTimeFormat( GetThreadLocale(), TIME_NOSECONDS, pSystemTime, NULL, pResult + len, (int)(maxResult - len) );
	return S_OK;
}
//_______________________________________________________________________________________________________________________________
//_______________________________________________________________________________________________________________________________
//_______________________________________________________________________________________________________________________________
ADDIN_API HRESULT WINAPI AddIn_SystemTime( DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/ )
{
	SYSTEMTIME SysTime;
	DWORD nGot;

	// read system time from debuggee memory space
	HRESULT hr = pHelper->ReadDebuggeeMemoryEx(pHelper, pHelper->GetRealAddress(pHelper), sizeof(SysTime), &SysTime, &nGot);
	if (hr != S_OK || nGot != sizeof(SysTime) || FormatDateTime( &SysTime, pResult, maxResult) != S_OK)
		sprintf_s(pResult, maxResult, "Error while formating variable!");
	return S_OK;
}
//_______________________________________________________________________________________________________________________________
ADDIN_API HRESULT WINAPI AddIn_FileTime( DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/ )
{
	FILETIME FileTime;
	SYSTEMTIME SysTime;
	DWORD nGot;

	// read file time from debuggee memory space
	HRESULT hr = pHelper->ReadDebuggeeMemoryEx(pHelper, pHelper->GetRealAddress(pHelper), sizeof(FileTime), &FileTime, &nGot);
	if (hr != S_OK || nGot != sizeof(FileTime) || !FileTimeToSystemTime( &FileTime, &SysTime ) || FormatDateTime( &SysTime, pResult, maxResult) != S_OK)
		sprintf_s(pResult, maxResult, "Error while formating variable!");

	return S_OK;
}
//_______________________________________________________________________________________________________________________________
ADDIN_API HRESULT WINAPI AddIn_COleDateTime( DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/ )
{
	COleDateTime datetime;
	DWORD nGot;

	// read system time from debuggee memory space
	HRESULT hr = pHelper->ReadDebuggeeMemoryEx(pHelper, pHelper->GetRealAddress(pHelper), sizeof(datetime), &datetime, &nGot);
	if (hr != S_OK || nGot != sizeof(datetime))
		sprintf_s(pResult, maxResult, "Error while formating variable!");
	else 
		sprintf_s(pResult, maxResult, datetime.GetStatus() == COleDateTime::valid ? datetime.Format() : "Invalid DateTime object.");

	return S_OK;
}
//_______________________________________________________________________________________________________________________________
ADDIN_API HRESULT WINAPI AddIn_COleDateTimeSpan( DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/ )
{
	COleDateTimeSpan dtSpan;
	DWORD nGot;

	// read system time from debuggee memory space
	HRESULT hr = pHelper->ReadDebuggeeMemoryEx(pHelper, pHelper->GetRealAddress(pHelper), sizeof(dtSpan), &dtSpan, &nGot);
	if (hr != S_OK || nGot != sizeof(dtSpan))
		sprintf_s(pResult, maxResult, "Error while formating variable!");
	else 
		sprintf_s(pResult, maxResult, dtSpan.GetStatus() == COleDateTimeSpan::valid ? dtSpan.Format("Days: %D, Hours: %H, Minutes: %M, Seconds: %S") : "Invalid span object.");

	return S_OK;
}
//_______________________________________________________________________________________________________________________________
//_______________________________________________________________________________________________________________________________
//_______________________________________________________________________________________________________________________________
