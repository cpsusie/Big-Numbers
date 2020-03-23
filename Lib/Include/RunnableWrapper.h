#pragma once

#include "SafeRunnable.h"

class RunnableWrapper : public SafeRunnable {
private:
  Runnable &m_job;
public:
  RunnableWrapper(Runnable &job) : m_job(job) {
  }

  inline Runnable &getRunnable() const {
    return m_job;
  }
  UINT safeRun() {
    return m_job.run();
  }
};

template<typename RunnableType> class RunnableWrapperTemplate : public RunnableWrapper {
public:
  RunnableWrapperTemplate(RunnableType &job) : RunnableWrapper(job) {
  }
  inline RunnableType &getJob() const {
    return (RunnableType&)getRunnable();
  }
};
