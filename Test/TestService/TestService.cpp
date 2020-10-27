#include "stdafx.h"

bool StopProgram = false;

void mainloop() {

  while(!StopProgram) {
    Sleep(1000);
  }
}

HANDLE  hServerStopEvent = nullptr;
VOID ServiceStart(DWORD dwArgc, LPTSTR *lpszArgv) {
  HANDLE hEvents[2] = {nullptr, nullptr};

  if(!ReportStatusToSCMgr(
    SERVICE_START_PENDING, // service state
    NO_ERROR,              // exit code
    3000))                 // wait hint
    goto cleanup;

  hServerStopEvent = CreateEvent(
    nullptr,    // no security attributes
    TRUE,    // manual reset event
    FALSE,   // not-signalled
    nullptr);   // no name

  if( hServerStopEvent == nullptr)
    goto cleanup;

  hEvents[0] = hServerStopEvent;

  if(!ReportStatusToSCMgr(
    SERVICE_START_PENDING, // service state
    NO_ERROR,              // exit code
    3000))                 // wait hint
    goto cleanup;

  hEvents[1] = CreateEvent(
    nullptr,    // no security attributes
    TRUE,    // manual reset event
    FALSE,   // not-signalled
    nullptr);   // no name

  if( hEvents[1] == nullptr)
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
