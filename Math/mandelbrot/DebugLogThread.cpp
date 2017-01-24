#include "stdafx.h"

#ifdef LOGDEBUGINFO

DebugLogThread DebugLogThread::thread;

void DebugLogThread::debugLog(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  m_cmdQueue.put(vformat(format,argptr));
  va_end(argptr);
}

UINT DebugLogThread::run() {
  for(;;) {
    const DebugThreadQueueElement qe = m_cmdQueue.get();
    switch(qe.m_cmd) {
    case RESETFILE:  ::redirectDebugLog();         break;
    case PRINTSTR:   ::debugLog(_T("%s"), qe.cstr()); break;
    }
  }
  return 0;
}

#endif
