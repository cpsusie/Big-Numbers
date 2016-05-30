#include "pch.h"
#include <Semaphore.h>

Semaphore::Semaphore(int initialcount, int maxwait) {
  if((m_sem = CreateSemaphore(NULL,initialcount,maxwait,NULL)) == NULL) {
    throwLastErrorOnSysCallException(_T("CreateSemaphore"));
  }
}

Semaphore::~Semaphore() {
  CloseHandle(m_sem);
}

bool Semaphore::wait(int milliseconds) {
  const int ret = WaitForSingleObject(m_sem, milliseconds);
  switch(ret) {
  case WAIT_OBJECT_0: return true;
  case WAIT_TIMEOUT : return false;
  case WAIT_FAILED  : throwLastErrorOnSysCallException(_T("WaitForSingleObject"));
  default           : throwException(_T("Semaphore:wait:Unexpected returncode:%d"), ret); return false;
  }
}

void Semaphore::signal() {
  ReleaseSemaphore(m_sem, 1, NULL);
}

static void addDebugLine(const TCHAR *format, ...) {
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
  gate.signal();
}

void Semaphore::wait(const char *name, const char *file, int line) {
  addDebugLine(_T("Thread %08d %s(%d).wait()   in %s line %d\n")
               ,GetCurrentThreadId(), name, m_sem, file, line);
  wait();
}

void Semaphore::signal(const char *name, const char *file, int line) {
  addDebugLine(_T("Thread %08d %s(%d).signal() in %s line %d\n")
               ,GetCurrentThreadId(), name, m_sem, file, line);
  signal();
}

