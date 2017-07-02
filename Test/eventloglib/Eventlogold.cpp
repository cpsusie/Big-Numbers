#include <stdio.h>
#include "eventlog.h"
#include "message.h"
#include <direct.h>
#include <afxwin.h>
#include <afxdisp.h>

static BOOL AddEventSource(
   LPTSTR pszLogName, // "Application" or a custom logfile name
   LPTSTR pszSrcName, // event source name
   LPTSTR pszMsgDLL,  // path for message DLL
   DWORD  dwNum)      // number of categories
{
   HKEY hk;
   DWORD dwData;
   TCHAR szBuf[MAX_PATH];

   // Create the event source as a subkey of the logfile.

   wsprintf(szBuf,
      "SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s\\%s",
      pszLogName, pszSrcName);

   if (RegCreateKey(HKEY_LOCAL_MACHINE, szBuf, &hk))
   {
//      printf("Could not create the registry key.");
      return FALSE;
   }

   // Set the name of the message file.

   if(RegSetValueEx(hk,              // subkey handle
           "EventMessageFile",        // value name
           0,                         // must be zero
           REG_EXPAND_SZ,             // value type
           (LPBYTE) pszMsgDLL,        // pointer to value data
           (DWORD) lstrlen(szBuf)+1) != ERROR_SUCCESS ) // length of value data
   {
//      printf("Could not set the event message file.");
      return FALSE;
   }

   // Set the supported event types.

   dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE |
        EVENTLOG_INFORMATION_TYPE;

   if(RegSetValueEx(hk,      // subkey handle
           "TypesSupported",  // value name
           0,                 // must be zero
           REG_DWORD,         // value type
           (LPBYTE) &dwData,  // pointer to value data
           sizeof(DWORD)))    // length of value data
   {
//      printf("Could not set the supported types.");
      return FALSE;
   }

   // Set the category message file and number of categories.

   if(RegSetValueEx(hk,              // subkey handle
           "CategoryMessageFile",     // value name
           0,                         // must be zero
           REG_EXPAND_SZ,             // value type
           (LPBYTE) pszMsgDLL,        // pointer to value data
           (DWORD) lstrlen(szBuf)+1) != ERROR_SUCCESS) // length of value data
   {
//      printf("Could not set the category message file.");
      return FALSE;
   }

   if(RegSetValueEx(hk,      // subkey handle
           "CategoryCount",   // value name
           0,                 // must be zero
           REG_DWORD,         // value type
           (LPBYTE) &dwNum,   // pointer to value data
           sizeof(DWORD)) != ERROR_SUCCESS )    // length of value data
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
   TCHAR szBuf[MAX_PATH];

   wsprintf(szBuf,
      "SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s\\%s",
      pszLogName, pszSrcName);
  return RegDeleteKey(HKEY_LOCAL_MACHINE,szBuf) == ERROR_SUCCESS;
}

static char modulefilename[_MAX_FNAME];

bool registereventsource() {
  char path[256];
  char drive[_MAX_DRIVE];
  char dir[_MAX_DIR];
  char ext[_MAX_EXT];

  GetModuleFileName(NULL,path,sizeof(path));

  _splitpath( path, drive, dir, modulefilename, ext );


  return AddEventSource("Application",modulefilename,path,1)?true:false;
}

bool deregistereventsource() {
  return RemoveEventSource("Application",modulefilename);
}

class autoregister {
public:
  autoregister() {
    registereventsource();
  }
};

static autoregister dummy;

static void log(int wtype, const char *format, va_list argptr) {
  TCHAR   szMsg[256];
  HANDLE  hEventSource;
  LPTSTR  lpszStrings[2];

  hEventSource = RegisterEventSource(NULL, TEXT(modulefilename));

  vsprintf(szMsg, format, argptr);
  lpszStrings[0] = szMsg;

  if(hEventSource != NULL) {
    ReportEvent(hEventSource,      // handle of event source
        wtype,                     // event type
        0,                         // event category
        MYMSG,                     // event ID
        NULL,                      // current user's SID
        1,                         // strings in lpszStrings
        0,                         // no bytes of raw data
        (const char**)lpszStrings, // Array of error strings
        NULL);                     // no raw data

    DeregisterEventSource(hEventSource);
  }
}

void eventlogerror(const char *format,...) {
  va_list argptr;
  va_start(argptr,format);
  log(EVENTLOG_ERROR_TYPE,format, argptr);
  va_end(argptr);
}

void eventlogwarning(const char *format,...) {
  va_list argptr;
  va_start(argptr,format);
  log(EVENTLOG_WARNING_TYPE,format, argptr);
  va_end(argptr);
}

void eventloginfo(const char *format,...) {
  va_list argptr;
  va_start(argptr,format);
  log(EVENTLOG_INFORMATION_TYPE,format, argptr);
  va_end(argptr);
}

void eventlogsuccess(const char *format,...) {
  va_list argptr;
  va_start(argptr,format);
  log(EVENTLOG_SUCCESS,format, argptr);
  va_end(argptr);
}

