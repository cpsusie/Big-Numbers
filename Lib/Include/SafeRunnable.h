#pragma once

#include "Runnable.h"
#include "FlagTraits.h"
#include "FastSemaphore.h"

#define _SRJOB_STARTED 0x01
#define _SRJOB_RUNNING 0x02
#define _SRJOB_DONE    0x04
#define _SRJOB_ERROR   0x08

class SafeRunnable : public Runnable {
private:
  FastSemaphore         m_terminated, m_started;
  mutable FastSemaphore m_lock;
  String                m_errorMsg;
  ATOMICFLAGTRAITS(SafeRunnable, BYTE, m_flags);

  void setErrorMsg(const TCHAR *msg);
  void setJobStarted();
  void setJobDone();
  void waitUntilJobStarted();
public:
  SafeRunnable() : m_flags(0), m_started(0) {
  }
  ~SafeRunnable() {
    waitUntilJobDone();
  }

  SafeRunnable &waitUntilJobDone();
  inline bool isJobStarted() const {
    return isSet(_SRJOB_STARTED);
  }
  inline bool isJobRunning() const {
    return isSet(_SRJOB_RUNNING);
  }
  inline bool isJobDone() const {
    return isSet(_SRJOB_DONE);
  }
  inline bool isOk() const {
    return !isSet(_SRJOB_ERROR);
  }
  String getErrorMsg() const;
  // Final. Should not be overwritten
  // Call safeRun and return value from there, or catch all exceptions, which can be detected by isOk().
  // If exception is caught, the returncode is -1
  // To get message text from caught exception, call getErrorMsg();
  // If destructor is called, caller will wait, until run has terminated.
  UINT run();
  // Should do the real job. Will be called by run(), see this
  virtual UINT safeRun() = 0;
};
