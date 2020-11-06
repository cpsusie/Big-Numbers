#include "pch.h"
#include <MyUtil.h>
#include <SysSemaphore.h>
#if defined(TRACESEMAPHORE)
#include <Thread.h>
#endif  // TRACESEMAPHORE

SysSemaphore::SysSemaphore(const String &name, UINT initialCount, UINT maxWait) {
  if((m_sem = CreateSemaphore(nullptr, initialCount, maxWait, name.cstr())) == nullptr) {
    throwLastErrorOnSysCallException(_T("CreateSemaphore"));
  }
}

SysSemaphore::SysSemaphore(UINT initialCount, UINT maxWait) {
  if((m_sem = CreateSemaphore(nullptr,initialCount,maxWait,nullptr)) == nullptr) {
    throwLastErrorOnSysCallException(_T("CreateSemaphore"));
  }
}

SysSemaphore::~SysSemaphore() {
  CloseHandle(m_sem);
}

bool SysSemaphore::wait(UINT timeout) {
  const int ret = WaitForSingleObject(m_sem, timeout);
  switch(ret) {
  case WAIT_OBJECT_0:
    return true;
  case WAIT_TIMEOUT :
    return false;
  case WAIT_FAILED  :
    throwLastErrorOnSysCallException(_T("WaitForSingleObject"));
  default           :
    throwException(_T("%s:Unexpected returncode:%d"), __TFUNCTION__, ret);
    return false;
  }
}

void SysSemaphore::notify() {
  if(!ReleaseSemaphore(m_sem, 1, nullptr)) {
    throwLastErrorOnSysCallException(_T("ReleaseSemaphore"));
  }
}

#if defined(TRACESEMAPHORE)

static String getCurrentThreadIdentification() {
  try {
    return getThreadDescription(GetCurrentThread());
  } catch(...) {
    return format(_T("%08d"), GetCurrentThreadId());
  }
}

bool SysSemaphore::wait(const TCHAR *name, const TCHAR *file, int line, const TCHAR *function, UINT timeout) {
  debugLog(_T("%s:Thread %s:%s(%p).wait(%s) in %s(%d)\n")
          ,function, getCurrentThreadIdentification().cstr(), name, m_sem
          ,(timeout==INFINITE)?_T(""):format(_T("%u msec"),timeout).cstr()
          ,file, line);
  return wait(timeout);
}

void SysSemaphore::notify(const TCHAR *name, const TCHAR *file, int line, const TCHAR *function) {
  debugLog(_T("%s:Thread %s:%s(%p).notify() in %s(%d)\n")
          ,function, getCurrentThreadIdentification().cstr(), name, m_sem
          ,file, line);
  notify();
}
#endif  // TRACESEMAPHORE
