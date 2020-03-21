#pragma once

#include "Runnable.h"
#include "FlagTraits.h"
#include "FastSemaphore.h"

#define _RWJOB_STARTED 0x01
#define _RWJOB_RUNNING 0x02
#define _RWJOB_DONE    0x04
#define _RWJOB_ERROR   0x08

class RunnableWrapper : public Runnable {
private:
  FastSemaphore         m_terminated, m_started;
  mutable FastSemaphore m_lock;
  String                m_errorMsg;
  ATOMICFLAGTRAITS(BYTE, RunnableWrapper);
  Runnable             &m_job;

  void setErrorMsg(const TCHAR *msg);
  void setJobStarted();
  void setJobDone();
  void waitUntilJobStarted();
public:
  RunnableWrapper(Runnable &job) : m_job(job), m_flags(0), m_started(0) {
  }
  ~RunnableWrapper();

  RunnableWrapper &waitUntilJobDone();
  inline bool isJobStarted() const {
    return isSet(_RWJOB_STARTED);
  }
  inline bool isJobRunning() const {
    return isSet(_RWJOB_RUNNING);
  }
  inline bool isJobDone() const {
    return isSet(_RWJOB_DONE);
  }
  inline bool isOk() const {
    return !isSet(_RWJOB_ERROR);
  }
  inline Runnable &getRunnable() const {
    return m_job;
  }
  String getErrorMsg() const;
  UINT run();
};

template<typename RunnableType> class RunnableWrapperTemplate : public RunnableWrapper {
public:
  RunnableWrapperTemplate(RunnableType &job) : RunnableWrapper(job) {
  }
  inline RunnableType &getJob() const {
    return (RunnableType&)getRunnable();
  }
};
