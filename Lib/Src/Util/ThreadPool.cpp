#include "pch.h"
#include <CPUInfo.h>
#include <SingletonFactory.h>
#include "ThreadPoolInternal.h"

typedef enum {
  REQUEST_GETINSTANCE
 ,REQUEST_RELEASEALL
} POOLREQUEST;

DEFINESINGLETONFACTORY(ThreadPool);

void *ThreadPool::poolRequest(int request) {
  static ThreadPoolFactory factory;
  switch(request) {
  case REQUEST_GETINSTANCE:
    return &factory.getInstance();
  case REQUEST_RELEASEALL:
    factory.releaseInstance();
    break;
  }
  return NULL;
}

ThreadPool &ThreadPool::getInstance() { // static
  return *((ThreadPool*)poolRequest(REQUEST_GETINSTANCE));
}

void ThreadPool::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(Thread::isPropertyContainer(source)) {
    switch(id) {
    case THR_SHUTTINGDDOWN:
      TRACE(_T("%s:received THR_SHUTTINGDDOWN\n"), __TFUNCTION__);
      prepareDelete();
      break;

    case THR_THREADSRUNNING:
      { bool threadsRunning = *(bool*)newValue;
        TRACE(_T("%s:received THR_THREADSRUNNING=%s\n"), __TFUNCTION__, boolToStr(threadsRunning));
        if(!threadsRunning) {
          poolRequest(REQUEST_RELEASEALL);
        }
      }
    }
  }
}

void ThreadPool::prepareDelete() {
  wait();
  TRACE(_T("%s enter. m_blockExecute=%s\n"), __TFUNCTION__, boolToStr(m_blockExecute));
  if(!m_blockExecute) {
    TRACE(_T("%s:now prepare delete\n"), __TFUNCTION__);
    setProperty(THREADPOOL_SHUTTINGDDOWN, m_blockExecute, true);
    killLogger();
    m_threadPool->requestTerminateAll();
    TRACE(_T("%s:prepare delete done\n"), __TFUNCTION__);
  }
  TRACE(_T("%s leave\n"), __TFUNCTION__);
  notify();
}

void ThreadPool::addListener(PropertyChangeListener *listener) { // static
  ThreadPool &instance = getInstance();
  instance.wait();  // get exclusive access to ThreadPool
  instance.addPropertyChangeListener(listener);
  instance.notify(); // open gate for other threads
}

void ThreadPool::removeListener(PropertyChangeListener *listener) { // static
  ThreadPool &instance = getInstance();
  instance.wait();  // get exclusive access to ThreadPool
  instance.removePropertyChangeListener(listener);
  instance.notify(); // open gate for other threads
}

class PoolLogger : public Runnable {
  bool          m_stopped, m_killed;
  Semaphore     m_start;
  FastSemaphore m_terminated;
public:
  PoolLogger() : m_start(0), m_stopped(true), m_killed(false), m_terminated(0) {
  }
  void startLogging() {
    if(m_stopped) {
      m_stopped = false;
      m_start.notify();
    }
  }
  inline void stopLogging() {
    m_stopped = true;
  }
  inline void killLogging() {
    m_killed = true;
    startLogging();
    m_terminated.wait();
  }
  UINT run();
};


UINT PoolLogger::run() {
  for(;;) {
    const int timeout = m_stopped ? INFINITE : 2000;
    m_start.wait(timeout);
    if(m_killed) break;
    if(m_stopped) {
      continue;
    }
    TRACE(_T("%s\n"), ThreadPool::getInstance().toString().cstr());
    if(m_killed) break;
  }
  m_terminated.notify();
  return 0;
}

PropertyContainer *ThreadPool::s_propertySource = NULL;

ThreadPool::ThreadPool() {
  TRACE(_T("%s called\n"),__TFUNCTION__);
  m_processorCount = getProcessorCount();
  m_activeThreads  = m_maxActiveThreads = 1;
  m_blockExecute   = false;
  s_propertySource = this;
  m_threadPool = new PoolThreadPool;  TRACE_NEW(m_threadPool);
  m_queuePool  = new ResultQueuePool; TRACE_NEW(m_queuePool);
  Thread::addListener(this);
  TRACE(_T("ThreadPool attached to Thread::propertyChangeListener\n"));
  TRACE(_T("%s done\n"), __TFUNCTION__);
}

ThreadPool::~ThreadPool() {
  TRACE(_T("%s called\n"), __TFUNCTION__);
  prepareDelete();
  wait();
  Thread::removeListener(this);
  TRACE(_T("ThreadPool detached from Thread::propertyChangeListener\n"));
  SAFEDELETE(m_threadPool);
  SAFEDELETE(m_queuePool);
  SAFEDELETE(m_logger    );
  s_propertySource = NULL;
  notify();
  TRACE(_T("ThreadPool deleted\n"));
}

void ThreadPool::setPriority(int priority) { // static
  ThreadPool &instance = getInstance();
  instance.wait();
  PoolThreadPool &pool = instance.getTPool();
  try {
    if(priority != pool.getPriority()) {
      pool.setPriority(priority);
    }
  } catch(...) {
    instance.notify();
    throw;
  }
  instance.notify();
}

void ThreadPool::setPriorityBoost(bool disablePriorityBoost) { // static
  ThreadPool &instance = getInstance();
  instance.wait();
  PoolThreadPool &pool = instance.getTPool();
  try {
    if(disablePriorityBoost != pool.getPriorityBoost()) {
      pool.setPriorityBoost(disablePriorityBoost);
    }
  } catch(...) {
    instance.notify();
    throw;
  }
  instance.notify();
}

void ThreadPool::executeNoWait(Runnable &job) {
  ThreadPool &instance = getInstance();
  instance.wait();  // get exclusive access to ThreadPool
  if(!instance.m_blockExecute) {
    PoolThreadPool &pool = instance.getTPool();
    pool.fetchResource()->executeJob(job, NULL);
  }
  instance.notify(); // open gate for other threads
}

 // execute all jobs without blocking. Uncaught exceptions are lost.
void ThreadPool::executeInParallelNoWait(RunnableArray &jobs) { // static
  ThreadPool &instance = getInstance();
  instance.wait();  // get exclusive access to ThreadPool
  if(!instance.m_blockExecute) {
    PoolThreadPool &pool = instance.getTPool();
    for(size_t i = 0; i < jobs.size(); i++) {
      pool.fetchResource()->executeJob(*jobs[i], NULL);
    }
  }
  instance.notify(); // open gate for other threads
}

// Blocks until all jobs are done. If any of the jobs throws an exception
// the rest of the jobs will be terminated and an exception with the same
// message will be thrown to the caller
void ThreadPool::executeInParallel(RunnableArray &jobs) { // static
  if(jobs.size() == 0) {
    return;
  }
  ThreadPool &instance = getInstance();
  instance.wait();  // get exclusive access to ThreadPool
  if(instance.m_blockExecute) {
    instance.notify();
    throwException("ThreadPool is shutting down");
    return;
  }
  PoolThreadPool &Tpool = instance.getTPool();

  CompactArray<ThreadPoolThread*> threadArray(jobs.size());
  for(size_t i = 0; i < jobs.size(); i++) {
    threadArray.add(Tpool.fetchResource());
  }
  ThreadPoolResultQueue *queue = instance.getQPool().fetchResource();
  for(size_t i = 0; i < jobs.size(); i++) {
    threadArray[i]->executeJob(*jobs[i], queue);
  }
  instance.notify(); // open gate for other threads

  try {
    queue->waitForResults(jobs.size());
    instance.wait();
    instance.getQPool().releaseResource(queue);
    instance.notify();
  } catch(...) {
    instance.wait();
    instance.getQPool().releaseResource(queue);
    instance.notify();
    throw;
  }
}

void ThreadPool::releaseThread(ThreadPoolThread *thread) { // static
  ThreadPool &instance = getInstance();
  instance.wait();
  instance.getTPool().releaseResource(thread);
  instance.notify();
}

String ThreadPool::toString() { // static
  ThreadPool &instance = getInstance();
  String result;
  instance.wait();

  result = format(_T("Threads:%s Queues:%s")
                 ,instance.getTPool().toString().cstr()
                 ,instance.getQPool().toString().cstr()
                 );
  instance.notify();
  return result;
}

void ThreadPool::startLogging() { // static
  ThreadPool &instance = getInstance();
  instance.wait();

  if(instance.m_logger == NULL) {
    instance.m_logger = new PoolLogger(); TRACE_NEW(instance.m_logger);
  }
  instance.notify();
  instance.executeNoWait(*instance.m_logger);
}

void ThreadPool::stopLogging() {  // static
  ThreadPool &instance = getInstance();
  instance.wait();
  if(instance.m_logger != NULL) {
    instance.m_logger->stopLogging();
  }
  instance.notify();
}

void ThreadPool::killLogger() { // not static. no wait/notify.....we are called from propertyCahngeListener which has a lock on this
  if(m_logger) {
    m_logger->killLogging();
  }
}
