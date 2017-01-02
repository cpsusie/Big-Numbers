#include "pch.h"
#include <Thread.h>

void InterruptableRunnable::setInterrupted() {
  m_flags |= 2;
  if(isSuspended()) {
    resume();
  }
}

void InterruptableRunnable::die() {
  throwException(_T("Interrupted"));
}

void InterruptableRunnable::resume() {
  if(m_thr != INVALID_HANDLE_VALUE) {
    ResumeThread(m_thr);
  }
}

void InterruptableRunnable::suspend() {
  m_thr = getCurrentThreadHandle();
  SuspendThread(m_thr);
  CloseHandle(m_thr);
  m_thr = INVALID_HANDLE_VALUE;
  clrSuspended();
}
