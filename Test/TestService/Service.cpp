/*
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
 * ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.
 *
 *  MODULE:   service.c
 *  PURPOSE:  Implements functions required by all services windows.
 *
 *  FUNCTIONS:
 *    main(int argc, char **argv);
 *    serviceCtrl(DWORD dwCtrlCode);
 *    service_main(DWORD dwArgc, LPTSTR *lpszArgv);
 *    CmdInstallService();
 *    CmdRemoveService();
 *    CmdDebugService(int argc, char **argv);
 *    ControlHandler ( DWORD dwCtrlType );
 *    GetLastErrorText( LPTSTR lpszBuf, DWORD dwSize );
 *
 *  AUTHOR: Craig Link - Microsoft Developer Support
 *
*/

#include "stdafx.h"

// internal variables
SERVICE_STATUS          ssStatus;       // current status of the service
SERVICE_STATUS_HANDLE   sshStatusHandle;
DWORD                   dwErr = 0;
BOOL                    bDebug = FALSE;
TCHAR                   szErr[256];

// internal function prototypes
VOID WINAPI serviceCtrl(DWORD dwCtrlCode);
VOID WINAPI serviceMain(DWORD dwArgc, LPTSTR *lpszArgv);
VOID        CmdInstallService();
VOID        CmdRemoveService();
VOID        CmdDebugService(int argc, char **argv);
VOID        CmdKillChilds();
BOOL WINAPI ControlHandler(DWORD dwCtrlType);
LPTSTR      GetLastErrorText(LPTSTR lpszBuf, DWORD dwSize);

//  FUNCTION: main
//  PURPOSE : entrypoint for service
//  PARAMETERS:
//    argc - number of command line arguments
//    argv - Array of command line arguments
//  COMMENTS:
//    main() either performs the command line task, or
//    call StartServiceCtrlDispatcher to register the
//    main service thread.  When the this call returns,
//    the service has stopped, so exit.
void main(int argc, char **argv) {
  SERVICE_TABLE_ENTRY dispatchTable[] = {
    { TEXT(SZSERVICENAME), (LPSERVICE_MAIN_FUNCTION)serviceMain },
    { NULL, NULL }
  };

  if((argc > 1) && ((*argv[1] == '-') || (*argv[1] == '/'))) {
    if(_stricmp( "install", argv[1]+1 ) == 0 ) {
      CmdInstallService();
    } else if(_stricmp( "remove", argv[1]+1 ) == 0) {
      CmdRemoveService();
    } else if(_stricmp( "debug", argv[1]+1 ) == 0) {
      bDebug = TRUE;
      CmdDebugService(argc, argv);
    } else if(_stricmp( "kill", argv[1]+1) == 0) {
      CmdKillChilds();
    } else {
      goto Dispatch;
    }
    exit(0);
  }

  // if it doesn't match any of the above parameters
  // the service control manager may be starting the service
  // so we must call StartServiceCtrlDispatcher
Dispatch:
  // this is just to be friendly
  printf( "%s -install          to install the service\n", SZAPPNAME );
  printf( "%s -remove           to remove the service\n", SZAPPNAME );
  printf( "%s -debug <params>   to run as a console app for debugging\n", SZAPPNAME );
  printf( "\nStartServiceCtrlDispatcher being called.\n" );
  printf( "This may take several seconds.  Please wait.\n" );

  if (!StartServiceCtrlDispatcher(dispatchTable))
    AddToMessageLog(TEXT("StartServiceCtrlDispatcher failed."));
}

//  FUNCTION: serviceMain
//  PURPOSE: To perform actual initialization of the service
//  PARAMETERS:
//    dwArgc   - number of command line arguments
//    lpszArgv - Array of command line arguments
//  COMMENTS:
//    This routine performs the service initialization and then calls
//    the user defined ServiceStart() routine to perform majority
//    of the work.
void WINAPI serviceMain(DWORD dwArgc, LPTSTR *lpszArgv) {
  // register our service control handler:
  sshStatusHandle = RegisterServiceCtrlHandler( TEXT(SZSERVICENAME), serviceCtrl);

  if(!sshStatusHandle) {
    goto cleanup;
  }

  // SERVICE_STATUS members that don't change in example
  ssStatus.dwServiceType             = SERVICE_WIN32_OWN_PROCESS;
  ssStatus.dwServiceSpecificExitCode = 0;

  // report the status to the service control manager.
  if(!ReportStatusToSCMgr(SERVICE_START_PENDING // service state
                         ,NO_ERROR              // exit code
                         ,3000)) {              // wait hint
    goto cleanup;
  }

  ServiceStart( dwArgc, lpszArgv );

cleanup:

  // try to report the stopped status to the service control manager.
  if(sshStatusHandle) {
    ReportStatusToSCMgr(SERVICE_STOPPED
                       ,dwErr
                       ,0);
  }

  return;
}

//
//  FUNCTION: serviceCtrl
//  PURPOSE: This function is called by the SCM whenever
//           ControlService() is called on this service.
//  PARAMETERS:
//    dwCtrlCode - type of control requested
void WINAPI serviceCtrl(DWORD dwCtrlCode) {
  // Handle the requested control code.
  switch(dwCtrlCode) {
  case SERVICE_CONTROL_STOP: // Stop the service.
    ssStatus.dwCurrentState = SERVICE_STOP_PENDING;
    ServiceStop();
    break;
  case SERVICE_CONTROL_INTERROGATE: // Update the service status.
    break;
  default: // invalid control code
    break;
  }
  ReportStatusToSCMgr(ssStatus.dwCurrentState, NO_ERROR, 0);
}

//  FUNCTION: ReportStatusToSCMgr()
//  PURPOSE: Sets the current status of the service and
//           reports it to the Service Control Manager
//
//  PARAMETERS:
//    dwCurrentState - the state of the service
//    dwWin32ExitCode - error code to report
//    dwWaitHint - worst case estimate to next checkpoint
//
//  RETURN VALUE:
//    TRUE  - success
//    FALSE - failure
BOOL ReportStatusToSCMgr(DWORD dwCurrentState
                        ,DWORD dwWin32ExitCode
                        ,DWORD dwWaitHint)
{
  static DWORD dwCheckPoint = 1;
  BOOL fResult = TRUE;

  if(!bDebug) { // when debugging we don't report to the SCM
    if(dwCurrentState == SERVICE_START_PENDING) {
      ssStatus.dwControlsAccepted = 0;
    } else {
      ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    }

    ssStatus.dwCurrentState  = dwCurrentState;
    ssStatus.dwWin32ExitCode = dwWin32ExitCode;
    ssStatus.dwWaitHint      = dwWaitHint;

    if( ( dwCurrentState == SERVICE_RUNNING ) ||
        ( dwCurrentState == SERVICE_STOPPED ) )
      ssStatus.dwCheckPoint = 0;
    else
      ssStatus.dwCheckPoint = dwCheckPoint++;
    // Report the status of the service to the service control manager.
    if(!(fResult = SetServiceStatus( sshStatusHandle, &ssStatus))) {
      AddToMessageLog(TEXT("SetServiceStatus"));
    }
  }
  return fResult;
}

//
//  FUNCTION: AddToMessageLog(LPTSTR lpszMsg)
//  PURPOSE: Allows any thread to log an error message
//  PARAMETERS:
//    lpszMsg - text for message
void AddToMessageLog(LPTSTR lpszMsg) {
  HANDLE  hEventSource;
  TCHAR   szMsg[256];
  TCHAR  *lpszStrings[2];

  if(!bDebug) {
    dwErr = GetLastError();

    // Use event logging to log the error.
    hEventSource = RegisterEventSource(NULL, TEXT(SZSERVICENAME));

    _stprintf(szMsg, _T("%s error: %d"), TEXT(SZSERVICENAME), dwErr);
    lpszStrings[0] = szMsg;
    lpszStrings[1] = lpszMsg;

    if(hEventSource != NULL) {
      ReportEvent(hEventSource              // handle of event source
                 ,EVENTLOG_ERROR_TYPE       // event type
                 ,0                         // event category
                 ,0                         // event ID
                 ,NULL                      // current user's SID
                 ,2                         // strings in lpszStrings
                 ,0                         // no bytes of raw data
                 ,(LPCWSTR*)lpszStrings     // Array of error strings
                 ,NULL);                    // no raw data

      DeregisterEventSource(hEventSource);
    }
  }
}

///////////////////////////////////////////////////////////////////
//  The following code handles service installation and removal
//  FUNCTION: CmdInstallService()
//  PURPOSE: Installs the service
void CmdInstallService() {
  SC_HANDLE schService;
  SC_HANDLE schSCManager;
  TCHAR     szPath[512];

  if(GetModuleFileName( NULL, szPath, 512 ) == 0) {
    _tprintf(TEXT("Unable to install %s - %s\n"), TEXT(SZSERVICEDISPLAYNAME), GetLastErrorText(szErr, 256));
    return;
  }

  schSCManager = OpenSCManager(NULL                   // machine (NULL == local)
                              ,NULL                   // database (NULL == default)
                              ,SC_MANAGER_ALL_ACCESS  // access required
                              );
  if(!schSCManager) {
    _tprintf(TEXT("OpenSCManager failed - %s\n"), GetLastErrorText(szErr,256));
  } else {
    schService = CreateService(schSCManager               // SCManager database
                              ,TEXT(SZSERVICENAME)        // name of service
                              ,TEXT(SZSERVICEDISPLAYNAME) // name to display
                              ,SERVICE_ALL_ACCESS         // desired access
                              ,SERVICE_WIN32_OWN_PROCESS  // service type
                              ,SERVICE_DEMAND_START       // start type
                              ,SERVICE_ERROR_NORMAL       // error control type
                              ,szPath                     // service's binary
                              ,NULL                       // no load ordering group
                              ,NULL                       // no tag identifier
                              ,TEXT(SZDEPENDENCIES)       // dependencies
                              ,NULL                       // LocalSystem account
                              ,NULL);                     // no password

    if(!schService) {
      _tprintf(TEXT("CreateService failed - %s\n"), GetLastErrorText(szErr, 256));
    } else {
      _tprintf(TEXT("%s installed.\n"), TEXT(SZSERVICEDISPLAYNAME) );
      CloseServiceHandle(schService);
    }
    CloseServiceHandle(schSCManager);
  }
}

//  FUNCTION: CmdRemoveService()
//  PURPOSE: Stops and removes the service
void CmdRemoveService() {
  SC_HANDLE schService;
  SC_HANDLE schSCManager;

  schSCManager = OpenSCManager(NULL                    // machine (NULL == local)
                              ,NULL                    // database (NULL == default)
                              ,SC_MANAGER_ALL_ACCESS   // access required
                              );
  if(!schSCManager) {
    _tprintf(TEXT("OpenSCManager failed - %s\n"), GetLastErrorText(szErr,256));
  } else {
    schService = OpenService(schSCManager, TEXT(SZSERVICENAME), SERVICE_ALL_ACCESS);
    if(!schService) {
      _tprintf(TEXT("OpenService failed - %s\n"), GetLastErrorText(szErr,256));
    } else {
      // try to stop the service
      if(ControlService(schService, SERVICE_CONTROL_STOP, &ssStatus)) {
        _tprintf(TEXT("Stopping %s."), TEXT(SZSERVICEDISPLAYNAME));
        Sleep( 1000 );

        while(QueryServiceStatus(schService, &ssStatus)) {
          if(ssStatus.dwCurrentState == SERVICE_STOP_PENDING) {
            _tprintf(TEXT("."));
            Sleep( 1000 );
          } else {
            break;
          }
        }

        if(ssStatus.dwCurrentState == SERVICE_STOPPED) {
          _tprintf(TEXT("\n%s stopped.\n"), TEXT(SZSERVICEDISPLAYNAME) );
        } else {
          _tprintf(TEXT("\n%s failed to stop.\n"), TEXT(SZSERVICEDISPLAYNAME) );
        }
      }

      // now remove the service
      if(DeleteService(schService) ) {
        _tprintf(TEXT("%s removed.\n"), TEXT(SZSERVICEDISPLAYNAME) );
      } else {
        _tprintf(TEXT("DeleteService failed - %s\n"), GetLastErrorText(szErr,256));
      }
      CloseServiceHandle(schService);
    }
    CloseServiceHandle(schSCManager);
  }
}

///////////////////////////////////////////////////////////////////
//
//  The following code is for running the service as a console app
//
//  FUNCTION: CmdDebugService(int argc, char ** argv)
//  PURPOSE: Runs the service as a console application
//  PARAMETERS:
//    argc - number of command line arguments
//    argv - Array of command line arguments
void CmdDebugService(int argc, char **argv) {
    int     dwArgc;
    LPTSTR *lpszArgv;

#ifdef UNICODE
    lpszArgv = CommandLineToArgvW(GetCommandLineW(), &dwArgc );
#else
    dwArgc   = argc;
    lpszArgv = argv;
#endif

  _tprintf(TEXT("Debugging %s.\n"), TEXT(SZSERVICEDISPLAYNAME));
  SetConsoleCtrlHandler(ControlHandler, TRUE);
  ServiceStart(dwArgc, lpszArgv);
}

//
//  FUNCTION: ControlHandler ( DWORD dwCtrlType )
//  PURPOSE: Handled console control events
//  PARAMETERS:
//    dwCtrlType - type of control event
//  RETURN VALUE:
//    True - handled
//    False - unhandled
//  COMMENTS:
BOOL WINAPI ControlHandler(DWORD dwCtrlType) {
  switch( dwCtrlType ) {
  case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to simulate
  case CTRL_C_EVENT:      // SERVICE_CONTROL_STOP in debug mode
    _tprintf(TEXT("Stopping %s.\n"), TEXT(SZSERVICEDISPLAYNAME));
    ServiceStop();
    return TRUE;
  }
  return FALSE;
}

//  FUNCTION: GetLastErrorText
//  PURPOSE: copies error message text to String
//  PARAMETERS:
//    lpszBuf - destination buffer
//    dwSize - size of buffer
//  RETURN VALUE:
//    destination buffer
LPTSTR GetLastErrorText(LPTSTR lpszBuf, DWORD dwSize) {
  DWORD  dwRet;
  LPTSTR lpszTemp = NULL;

  dwRet = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER 
                      | FORMAT_MESSAGE_FROM_SYSTEM 
                      | FORMAT_MESSAGE_ARGUMENT_ARRAY
                       ,NULL
                       ,GetLastError()
                       ,LANG_NEUTRAL
                       ,(LPTSTR)&lpszTemp
                       ,0
                       ,NULL );

  // supplied buffer is not long enough
  if( !dwRet || ( (long)dwSize < (long)dwRet+14 ) ) {
    lpszBuf[0] = TEXT('\0');
  } else {
    lpszTemp[lstrlen(lpszTemp)-2] = TEXT('\0');  //remove cr and newline character
    _stprintf(lpszBuf, TEXT("%s (0x%x)"), lpszTemp, GetLastError());
  }

  if(lpszTemp) {
    LocalFree((HLOCAL)lpszTemp);
  }
  return lpszBuf;
}

void CmdKillChilds() {
}
