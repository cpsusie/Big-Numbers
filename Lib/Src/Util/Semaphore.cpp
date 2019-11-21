#include "pch.h"
#include <MyUtil.h>
#include <Semaphore.h>

Semaphore::Semaphore(int initialCount, int maxWait) {
  if((m_sem = CreateSemaphore(NULL,initialCount,maxWait,NULL)) == NULL) {
    throwLastErrorOnSysCallException(_T("CreateSemaphore"));
  }
}

Semaphore::~Semaphore() {
  CloseHandle(m_sem);
}

bool Semaphore::wait(int milliseconds) {
  const int ret = WaitForSingleObject(m_sem, milliseconds);
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
  if (!ReleaseSemaphore(m_sem, 1, NULL)) {
    throwLastErrorOnSysCallException(_T("ReleaseSemaphore"));
  }
}

static void addDebugLine(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  static Semaphore gate;
  static TCHAR *mode = _T("w");
  gate.wait();
  FILE *f = FOPEN(_T("c:\\temp\\semaphore.log"),mode);
  mode = _T("a");
  va_list argptr;
  va_start(argptr,format);
  _vftprintf(f, format, argptr);
  va_end(argptr);
  fclose(f);
  gate.notify();
}

void Semaphore::wait(const TCHAR *name, const TCHAR *file, int line) {
  addDebugLine(_T("Thread %08d %s(%p).wait()   in %s line %d\n")
               ,GetCurrentThreadId(), name, m_sem, file, line);
  wait();
}

void Semaphore::notify(const TCHAR *name, const TCHAR *file, int line) {
  addDebugLine(_T("Thread %08d %s(%p).notify() in %s line %d\n")
               ,GetCurrentThreadId(), name, m_sem, file, line);
  notify();
}

