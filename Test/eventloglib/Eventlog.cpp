#include "StdAfx.h"
#include "eventlog.h"
#include "message.h"
#include <direct.h>

#pragma comment(lib,"Advapi32.lib")

static BOOL AddEventSource(
   LPTSTR pszLogName, // "Application" or a custom logfile name
   LPTSTR pszSrcName, // event source name
   LPTSTR pszMsgDLL,  // path for message DLL
   DWORD  dwNum)      // number of categories
{
   HKEY hk;
   DWORD dwData;

   // Create the event source as a subkey of the logfile.

   String keyName = format(_T("SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s\\%s")
                          ,pszLogName, pszSrcName);

   if (RegCreateKey(HKEY_LOCAL_MACHINE, keyName.cstr(), &hk)) {
//      printf("Could not create the registry key.");
      return FALSE;
   }

   // Set the name of the message file.

   if(RegSetValueEx(hk                       // subkey handle
                   ,_T("EventMessageFile")   // value name
                   ,0                        // must be zero
                   ,REG_EXPAND_SZ            // value type
                   ,(LPBYTE)pszMsgDLL        // pointer to value data
                   ,(DWORD)keyName.length()+1) != ERROR_SUCCESS ) // length of value data
   {
//      printf("Could not set the event message file.");
      return FALSE;
   }

   // Set the supported event types.

   dwData = EVENTLOG_ERROR_TYPE
          | EVENTLOG_WARNING_TYPE
          | EVENTLOG_INFORMATION_TYPE;

   if(RegSetValueEx(hk                     // subkey handle
                   ,_T("TypesSupported")   // value name
                   ,0                      // must be zero
                   ,REG_DWORD              // value type
                   ,(LPBYTE)&dwData        // pointer to value data
                   ,sizeof(DWORD)))        // length of value data
   {
//      printf("Could not set the supported types.");
      return FALSE;
   }

   // Set the category message file and number of categories.

   if(RegSetValueEx(hk                                         // subkey handle
                   ,_T("CategoryMessageFile")                  // value name
                   ,0                                          // must be zero
                   ,REG_EXPAND_SZ                              // value type
                   ,(LPBYTE)pszMsgDLL                          // pointer to value data
                   ,(DWORD)keyName.length()+1) != ERROR_SUCCESS) // length of value data
   {
//      printf("Could not set the category message file.");
      return FALSE;
   }

   if(RegSetValueEx(hk                    // subkey handle
                   ,_T("CategoryCount")   // value name
                   ,0                     // must be zero
                   ,REG_DWORD             // value type
                   ,(LPBYTE)&dwNum        // pointer to value data
                   ,sizeof(DWORD)) != ERROR_SUCCESS )    // length of value data
   {
//      printf("Could not set the category count.");
      return FALSE;
   }

   RegCloseKey(hk);
   return TRUE;
}

static bool RemoveEventSource(LPTSTR pszLogName, // "Application" or a custom logfile name
                              LPTSTR pszSrcName) // event source name
{
   const String keyName = format(_T("SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s\\%s")
                                ,pszLogName, pszSrcName);
  return RegDeleteKey(HKEY_LOCAL_MACHINE,keyName.cstr()) == ERROR_SUCCESS;
}

static TCHAR modulefilename[_MAX_FNAME];

bool registereventsource() {
  TCHAR path[256];
  TCHAR drive[_MAX_DRIVE];
  TCHAR dir[_MAX_DIR];
  TCHAR ext[_MAX_EXT];

  GetModuleFileName(NULL,path,ARRAYSIZE(path));
  _tsplitpath(path, drive, dir, modulefilename, ext);
  return AddEventSource(_T("Application"),modulefilename,path,1)?true:false;
}

bool deregistereventsource() {
  return RemoveEventSource(_T("Application"),modulefilename);
}

class autoregister {
public:
  autoregister() {
    registereventsource();
  }
};

static autoregister dummy;

static void vlog(int wtype, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  HANDLE  hEventSource;
  LPTSTR  lpszStrings[2];

  hEventSource = RegisterEventSource(NULL, modulefilename);

  String msg = vformat(format, argptr);
  lpszStrings[0] = msg.cstr();

  if(hEventSource != NULL) {
    ReportEvent(hEventSource               // handle of event source
               ,wtype                      // event type
               ,0                          // event category
               ,MYMSG                      // event ID
               ,NULL                       // current user's SID
               ,1                          // strings in lpszStrings
               ,0                          // no bytes of raw data
               ,(const TCHAR**)lpszStrings // Array of error strings
               ,NULL);                     // no raw data

    DeregisterEventSource(hEventSource);
  }
}

void eventLogError(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr, format);
  vlog(EVENTLOG_ERROR_TYPE, format, argptr);
  va_end(argptr);
}

void eventLogWarning(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr, format);
  vlog(EVENTLOG_WARNING_TYPE, format, argptr);
  va_end(argptr);
}

void eventLogInfo(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr, format);
  vlog(EVENTLOG_INFORMATION_TYPE, format, argptr);
  va_end(argptr);
}

void eventLogSuccess(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr, format);
  vlog(EVENTLOG_SUCCESS, format, argptr);
  va_end(argptr);
}
