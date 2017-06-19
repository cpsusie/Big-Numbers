#include "stdafx.h"

bool StopProgram = false;

void mainloop() {

  while(!StopProgram) {
    Sleep(1000);
  }
}

HANDLE  hServerStopEvent = NULL;
VOID ServiceStart(DWORD dwArgc, LPTSTR *lpszArgv) {
  HANDLE hEvents[2] = {NULL, NULL};

  if(!ReportStatusToSCMgr(
    SERVICE_START_PENDING, // service state
    NO_ERROR,              // exit code
    3000))                 // wait hint
    goto cleanup;

  hServerStopEvent = CreateEvent(
    NULL,    // no security attributes
    TRUE,    // manual reset event
    FALSE,   // not-signalled
    NULL);   // no name

  if( hServerStopEvent == NULL)
    goto cleanup;

  hEvents[0] = hServerStopEvent;

  if(!ReportStatusToSCMgr(
    SERVICE_START_PENDING, // service state
    NO_ERROR,              // exit code
    3000))                 // wait hint
    goto cleanup;

  hEvents[1] = CreateEvent(
    NULL,    // no security attributes
    TRUE,    // manual reset event
    FALSE,   // not-signalled
    NULL);   // no name

  if( hEvents[1] == NULL)
    goto cleanup;

  if(!ReportStatusToSCMgr(
    SERVICE_RUNNING,       // service state
    NO_ERROR,              // exit code
    0))                    // wait hint
    goto cleanup;

  mainloop();

cleanup:

  if(hServerStopEvent)
    CloseHandle(hServerStopEvent);

  if(hEvents[1]) // overlapped i/o event
    CloseHandle(hEvents[1]);

}

VOID ServiceStop() {
  if( hServerStopEvent ) {
    SetEvent(hServerStopEvent);
    StopProgram = true;
  }
}
