#pragma once

#include "MyUtil.h"

typedef enum {    // Threadpriorities defined in winbase.h
  PRIORITY_IDLE          = THREAD_PRIORITY_IDLE
 ,PRIORITY_LOWEST        = THREAD_PRIORITY_LOWEST
 ,PRIORITY_BELOW_NORMAL  = THREAD_PRIORITY_BELOW_NORMAL
 ,PRIORITY_NORMAL        = THREAD_PRIORITY_NORMAL
 ,PRIORITY_ABOVE_NORMAL  = THREAD_PRIORITY_ABOVE_NORMAL
 ,PRIORITY_TIME_CRITICAL = THREAD_PRIORITY_TIME_CRITICAL
 ,PRIORITY_HIGHEST       = THREAD_PRIORITY_HIGHEST
} ThreadPriority;

// call DuplicateHandle(GetCurrentThread())
// returned Handle should be closed with CloseHandle() after use.
// Throw Exception on failure
HANDLE getCurrentThreadHandle();

// if hThread is not specified, currentThreadHandle is used
void   setThreadDescription(const String &description, HANDLE hThread = INVALID_HANDLE_VALUE);
// if hThread is not specified, currentThreadHandle is used
String getThreadDescription(HANDLE hThread = INVALID_HANDLE_VALUE);

#if defined(_DEBUG)
#define SETTHREADDESCRIPTION(description, ...) setThreadDescription(description, __VA_ARGS__)
#else
#define SETTHREADDESCRIPTION(description, ...)
#endif // _DEBUG

// if hThread is not specified, currentThreadHandle is used
void setThreadPriority(ThreadPriority priority, HANDLE hThread = INVALID_HANDLE_VALUE);
// if hThread is not specified, currentThreadHandle is used
ThreadPriority getThreadPriority(HANDLE hThread = INVALID_HANDLE_VALUE);

void setThreadPriorityBoost(bool disablePriorityBoost, HANDLE hThread = INVALID_HANDLE_VALUE);
bool getThreadPriorityBoost(HANDLE hThread = INVALID_HANDLE_VALUE);

void setThreadAffinityMask(  DWORD mask, HANDLE hThread = INVALID_HANDLE_VALUE);
void setThreadIdealProcessor(DWORD cpu , HANDLE hThread = INVALID_HANDLE_VALUE);
ULONG getThreadExitCode(HANDLE hThread = INVALID_HANDLE_VALUE);

// microseconds. if thread  == INVALID_HANDLE_VALUE, return time for current Thread
double getThreadTime(HANDLE hThread = INVALID_HANDLE_VALUE);
