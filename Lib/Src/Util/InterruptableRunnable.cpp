#include "pch.h"
#include <Runnable.h>

void InterruptableRunnable::die() {
  throwException(_T("Interrupted"));
}

void InterruptableRunnable::suspend() {
  m_thr    = GetCurrentThread();
  SuspendThread(m_thr);
  m_thr    = NULL;
}
