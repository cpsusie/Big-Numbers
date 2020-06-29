#include "pch.h"
#include <CPUInfo.h>
#include "ThreadPoolInternal.h"
#include <InterruptableRunnable.h>

#if defined(TRACE_THREADPOOL)
void threadPoolTrace(const TCHAR *function, const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  String msg = vformat(format, argptr);
  va_end(argptr);
  debugLog(_T("%-20s:%s\n"), function, msg.cstr());
}
#endif // TRACE_THREADPOOL

void ThreadPool::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(Thread::isPropertyContainer(source)) {
    switch(id) {
    case THR_BLOCKNEWTHREADS:
      THREADPOOL_TRACE("%s:received THR_BLOCKNEWTHREADS\n", __TFUNCTION__);
      prepareDelete();
      break;

    case THR_THREADSRUNNING:
      { const bool threadsRunning = *(bool*)newValue;
        THREADPOOL_TRACE("%s:received THR_THREADSRUNNING=%s\n", __TFUNCTION__, boolToStr(threadsRunning));
      }
      break;
    }
  }
}

// no lock
ThreadPool &ThreadPool::ExecuteNoWait(Runnable &job) {
  if(!m_blockExecute) {
    PoolThreadPool &pool = GetTPool();
    pool.fetchResource()->executeJob(job, NULL);
  }
  return *this;
}

// no lock
ThreadPool &ThreadPool::ExecuteInParallelNoWait(RunnableArray &jobs) {
  if(!m_blockExecute) {
    PoolThreadPool &pool = GetTPool();
    for(size_t i = 0; i < jobs.size(); i++) {
      pool.fetchResource()->executeJob(*jobs[i], NULL);
    }
  }
  return *this;
}

// no lock
ThreadPool &ThreadPool::PrepareDelete() {
  THREADPOOL_TRACE("%s enter. m_blockExecute=%s\n", __TFUNCTION__, boolToStr(m_blockExecute));
  if(!m_blockExecute) {
    THREADPOOL_TRACE("%s:now prepare delete\n", __TFUNCTION__);
    setProperty(THREADPOOL_SHUTTINGDDOWN, m_blockExecute, true);
    KillLogger();
    m_threadPool->requestTerminateAll();
    THREADPOOL_TRACE("%s:prepare delete done\n", __TFUNCTION__);
  }
  THREADPOOL_TRACE("%s leave\n", __TFUNCTION__);
  return *this;
}

// no lock
ThreadPool &ThreadPool::ReleaseThread(ThreadPoolThread *thread) {
  GetTPool().releaseResource(thread);
  return *this;
}

// no lock
ThreadPool &ThreadPool::AddListener(PropertyChangeListener *listener) {
  addPropertyChangeListener(listener);
  return *this;
}

// no lock
ThreadPool &ThreadPool::RemoveListener(PropertyChangeListener *listener) {
  removePropertyChangeListener(listener);
  return *this;
}

void ThreadPool::addListener(PropertyChangeListener *listener) { // static
  getInstance().wait().AddListener(listener).notify();
}

void ThreadPool::removeListener(PropertyChangeListener *listener) { // static
  getInstance().wait().RemoveListener(listener).notify();
}

class PoolLogger : public InterruptableRunnable {
private:
  TimedSemaphore m_log;
public:
  PoolLogger() : m_log(0) {
  }
  void setInterrupted() {
    __super::setInterrupted();
    m_log.notify();
  }
  UINT safeRun() override;
};

UINT PoolLogger::safeRun() {
  for(;;) {
    handleInterruptOrSuspend();
    m_log.wait(2000);
    handleInterruptOrSuspend();
    THREADPOOL_TRACE("%s\n", ThreadPool::getInstance().toString().cstr());
  }
  return 0;
}

PropertyContainer *ThreadPool::s_propertySource = NULL;

ThreadPool::ThreadPool() : Singleton(__TFUNCTION__), m_processorCount(getProcessorCount()) {
  THREADPOOL_ENTER;
  s_propertySource = this;
  Thread::getMap(); // to create ThreadMap is before this is created. then it will automatic be destroyed after this
  m_activeThreads  = m_maxActiveThreads = 1;
  m_blockExecute   = false;
  m_threadPool     = new PoolThreadPool(this);  TRACE_NEW(m_threadPool);
  m_queuePool      = new ResultQueuePool(this); TRACE_NEW(m_queuePool );
  Thread::addListener(this);
  THREADPOOL_TRACE("ThreadPool attached to Thread::propertyChangeListener\n");
  THREADPOOL_LEAVE;
}

ThreadPool::~ThreadPool() {
  THREADPOOL_ENTER;
  prepareDelete();
  wait();
  Thread::removeListener(this);
  SAFEDELETE(m_threadPool);
  SAFEDELETE(m_queuePool );
  SAFEDELETE(m_logger    );
  s_propertySource = NULL;
  notify();
  THREADPOOL_LEAVE;
}

void ThreadPool::setPriority(ThreadPriority priority) { // static
  ThreadPool     &instance = getInstance().wait();
  PoolThreadPool &pool     = instance.GetTPool();
  try {
    if(priority != pool.getPriority()) {
      pool.setPriority(priority);
    }
    instance.notify();
  } catch(...) {
    instance.notify();
    throw;
  }
}

void ThreadPool::setPriorityBoost(bool disablePriorityBoost) { // static
  ThreadPool     &instance = getInstance().wait();
  PoolThreadPool &pool     = instance.GetTPool();
  try {
    if(disablePriorityBoost != pool.getPriorityBoost()) {
      pool.setPriorityBoost(disablePriorityBoost);
    }
    instance.notify();
  } catch(...) {
    instance.notify();
    throw;
  }
}

void ThreadPool::prepareDelete() {
  wait().PrepareDelete().notify();
}

void ThreadPool::executeNoWait(Runnable &job) { // static
  getInstance().wait().ExecuteNoWait(job).notify();
}

 // execute all jobs without blocking. Uncaught exceptions are lost.
void ThreadPool::executeInParallelNoWait(RunnableArray &jobs) { // static
  getInstance().wait().ExecuteInParallelNoWait(jobs).notify();
}

// Blocks until all jobs are done. If any of the jobs throws an exception
// the rest of the jobs will be terminated and an exception with the same
// message will be thrown to the caller
void ThreadPool::executeInParallel(RunnableArray &jobs) { // static
  if(jobs.size() == 0) {
    return;
  }
  ThreadPool &instance = getInstance().wait();
  if(instance.m_blockExecute) {
    instance.notify();
    throwException("ThreadPool is shutting down");
    return;
  }
  PoolThreadPool &Tpool = instance.GetTPool();

  CompactArray<ThreadPoolThread*> threadArray(jobs.size());
  for(size_t i = 0; i < jobs.size(); i++) {
    threadArray.add(Tpool.fetchResource());
  }
  ThreadPoolResultQueue *queue = instance.GetQPool().fetchResource();
  for(size_t i = 0; i < jobs.size(); i++) {
    threadArray[i]->executeJob(*jobs[i], queue);
  }
  instance.notify(); // open gate for other threads

  try {
    queue->waitForResults(jobs.size());
    instance.wait().GetQPool().releaseResource(queue);
    instance.notify();
  } catch(...) {
    instance.wait().GetQPool().releaseResource(queue);
    instance.notify();
    throw;
  }
}

void ThreadPool::releaseThread(ThreadPoolThread *thread) {
  wait().ReleaseThread(thread).notify();
}

String ThreadPool::toString() const {
  m_gate.wait();
  const String result = format(_T("Threads:%s Queues:%s")
                              ,GetTPool().toString().cstr()
                              ,GetQPool().toString().cstr()
                              );
  m_gate.notify();
  return result;
}

void ThreadPool::startLogging() {
  wait();

  if(m_logger == NULL) {
    m_logger = new PoolLogger(); TRACE_NEW(m_logger);
    ExecuteNoWait(*m_logger);
  } else {
    m_logger->resume();
  }
  notify();
}

void ThreadPool::stopLogging() {
  wait();
  if(m_logger != NULL) {
    m_logger->setSuspended();
  }
  notify();
}

// no lock
ThreadPool &ThreadPool::KillLogger() { // not static. no wait/notify.....we are called from propertyChangeListener which has a lock on this
  if(m_logger) {
    m_logger->setInterrupted();
  }
  return *this;
}
