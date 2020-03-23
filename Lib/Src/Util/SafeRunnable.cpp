#include "pch.h"
#include <SafeRunnable.h>

void SafeRunnable::setErrorMsg(const TCHAR *msg) {
  m_lock.wait();
  m_errorMsg = msg;
  setFlag(_SRJOB_ERROR);
  m_lock.notify();
}

void SafeRunnable::setJobStarted() {
  m_lock.wait();
  if(isSet(_SRJOB_STARTED)) {
    m_lock.notify();
    return;
  }
  m_terminated.wait();
  setFlag(_SRJOB_STARTED | _SRJOB_RUNNING);
  m_lock.notify();
  m_started.notify();
}

void SafeRunnable::setJobDone() {
  m_lock.wait();
  if(isSet(_SRJOB_DONE)) {
    m_lock.wait();
    return;
  }
  setFlag(_SRJOB_DONE).clrFlag(_SRJOB_RUNNING);
  m_lock.notify();
  m_terminated.notify();
}

void SafeRunnable::waitUntilJobStarted() {
  m_lock.wait();
  if(!isJobStarted()) {
    m_lock.notify();
    m_started.wait();
    return;
  }
  m_lock.notify();
}

SafeRunnable &SafeRunnable::waitUntilJobDone() {
  m_lock.wait();
  if(!isJobDone()) {
    m_lock.notify();
    m_terminated.wait();
    return *this;
  }
  m_lock.notify();
  return *this;
}

String SafeRunnable::getErrorMsg() const {
  m_lock.wait();
  const String result = m_errorMsg;
  m_lock.notify();
  return result;
}

UINT SafeRunnable::run() {
  setJobStarted();
  UINT result;
  try {
    result = safeRun();
  } catch(Exception e) {
    setErrorMsg(e.what());
    result = -1;
  } catch(...) {
    result = -1;
    setErrorMsg(_T("Unknown exception"));
  }
  setJobDone();
  return result;
}
