#include "pch.h"
#include <RunnableWrapper.h>
#include <DebugLog.h>

RunnableWrapper::~RunnableWrapper() {
  waitUntilJobDone();
}

void RunnableWrapper::setErrorMsg(const TCHAR *msg) {
  m_lock.wait();
  m_errorMsg = msg;
  setFlag(_RWJOB_ERROR);
  m_lock.notify();
}
void RunnableWrapper::setJobStarted() {
  m_lock.wait();
  m_terminated.wait();
  setFlag(_RWJOB_STARTED | _RWJOB_RUNNING);
  m_started.notify();
  m_lock.notify();
}

void RunnableWrapper::setJobDone() {
  m_lock.wait();
  clrFlag(_RWJOB_RUNNING).setFlag(_RWJOB_DONE);
  m_terminated.notify();
  m_lock.notify();
}

void RunnableWrapper::waitUntilJobStarted() {
  m_lock.wait();
  if(!isJobStarted()) {
    m_lock.notify();
    m_started.wait();
    return;
  }
  m_lock.notify();
}

RunnableWrapper &RunnableWrapper::waitUntilJobDone() {
  m_lock.wait();
  if(!isJobDone()) {
    m_lock.notify();
    m_terminated.wait();
    return *this;
  }
  m_lock.notify();
  return *this;
}

String RunnableWrapper::getErrorMsg() const {
  m_lock.wait();
  const String result = m_errorMsg;
  m_lock.notify();
  return result;
}

UINT RunnableWrapper::run() {
  setJobStarted();
  UINT result;
  try {
    result = m_job.run();
  } catch (Exception e) {
    setErrorMsg(e.what());
    result = -1;
  } catch (...) {
    result = -1;
    setErrorMsg(_T("Unknown exception"));
  }
  setJobDone();
  return result;
}
