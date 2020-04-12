#include "pch.h"
#include <MyUtil.h>
#include <Semaphore.h>
#ifdef TRACESEMAPHORE
#include <Thread.h>
#include <DebugLog.h>
#endif  // TRACESEMAPHORE

Semaphore::Semaphore(const String &name, UINT initialCount, UINT maxWait) {
  if((m_sem = CreateSemaphore(NULL, initialCount, maxWait, name.cstr())) == NULL) {
    throwLastErrorOnSysCallException(_T("CreateSemaphore"));
  }
}

Semaphore::Semaphore(UINT initialCount, UINT maxWait) {
  if((m_sem = CreateSemaphore(NULL,initialCount,maxWait,NULL)) == NULL) {
    throwLastErrorOnSysCallException(_T("CreateSemaphore"));
  }
}

Semaphore::~Semaphore() {
  CloseHandle(m_sem);
}

bool Semaphore::wait(UINT timeout) {
  const int ret = WaitForSingleObject(m_sem, timeout);
  switch(ret) {
  case WAIT_OBJECT_0:
    return true;
  case WAIT_TIMEOUT :
    return false;
  case WAIT_FAILED  :
    throwLastErrorOnSysCallException(_T("WaitForSingleObject"));
  default           :
    throwException(_T("Semaphore:wait:Unexpected returncode:%d"), ret);
    return false;
  }
}

void Semaphore::notify() {
  if(!ReleaseSemaphore(m_sem, 1, NULL)) {
    throwLastErrorOnSysCallException(_T("ReleaseSemaphore"));
  }
}

#ifdef TRACESEMAPHORE

static String getCurrentThreadIdentification() {
  try {
    return getThreadDescription(GetCurrentThread());
  } catch(...) {
    return format(_T("%08d"), GetCurrentThreadId());
  }
}

bool Semaphore::wait(const TCHAR *name, const TCHAR *file, int line, const TCHAR *function, UINT timeout) {
  debugLog(_T("%s:Thread %s:%s(%p).wait(%s) in %s(%d)\n")
          ,function, getCurrentThreadIdentification().cstr(), name, m_sem
          ,(timeout==INFINITE)?_T(""):format(_T("%u msec"),timeout).cstr()
          ,file, line);
  return wait(timeout);
}

void Semaphore::notify(const TCHAR *name, const TCHAR *file, int line, const TCHAR *function) {
  debugLog(_T("%s:Thread %s:%s(%p).notify() in %s(%d)\n")
          ,function, getCurrentThreadIdentification().cstr(), name, m_sem
          ,file, line);
  notify();
}
#endif  // TRACESEMAPHORE
