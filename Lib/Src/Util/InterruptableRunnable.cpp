#include "pch.h"
#include <Thread.h>
#include <InterruptableRunnable.h>

void InterruptableRunnable::setInterrupted() {
  setFlag(_IRFLG_INTERRUPTED);
  resume();
}

void InterruptableRunnable::die(const TCHAR *msg) {
  throwException(_T("%s"), msg ? msg : _T("Interrupted"));
}

void InterruptableRunnable::resume() {
  if(isPaused()) {
    ResumeThread(m_thr);
  }
}

void InterruptableRunnable::saveHandle() {
  m_thr = getCurrentThreadHandle();
}
InterruptableRunnable &InterruptableRunnable::clearHandle() {
  CloseHandle(m_thr);
  m_thr = INVALID_HANDLE_VALUE;
  return *this;
}

void InterruptableRunnable::suspend() {
  saveHandle();
  SuspendThread(m_thr);
  if(clearHandle().clrSuspended().isInterrupted()) {
    die();
  }
}

void InterruptableRunnable::handleInterruptOrSuspend() {
  if(isInterrupted()) {
    die();
  } else if(isSuspended()) {
    suspend();
  }
}
