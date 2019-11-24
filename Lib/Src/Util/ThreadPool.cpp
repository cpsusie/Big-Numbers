#include "pch.h"
#include <CPUInfo.h>
#include <SingletonFactory.h>
#include "ThreadPoolInternal.h"
#include <DebugLog.h>

ThreadPool &ThreadPool::getInstance() {
  static SingletonFactoryTemplate<ThreadPool> factory;
  return factory.getInstance();
}

ThreadPool::ThreadPool() {
  m_processorCount = getProcessorCount();
  m_activeThreads  = m_maxActiveThreads = 1;
  m_threadPool = new IdentifiedThreadPool;      TRACE_NEW(m_threadPool);
  m_queuePool  = new IdentifiedResultQueuePool; TRACE_NEW(m_queuePool);
}

ThreadPool::~ThreadPool() {
  killLogging();
  wait();
  m_queuePool->deleteAll();
  SAFEDELETE(m_queuePool );
  SAFEDELETE(m_threadPool);
  SAFEDELETE(m_logger    );
  notify();
}

void ThreadPool::setPriority(int priority) { // static
  ThreadPool &instance = getInstance();
  instance.wait();
  IdentifiedThreadPool &pool = instance.getTPool();
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
  IdentifiedThreadPool &pool = instance.getTPool();
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
  IdentifiedThreadPool &pool = instance.getTPool();
  pool.fetchResource()->executeJob(job, NULL);
  instance.notify(); // open gate for other threads
}

 // execute all jobs without blocking. Uncaught exceptions are lost.
void ThreadPool::executeInParallelNoWait(RunnableArray &jobs) { // static
  ThreadPool &instance = getInstance();
  instance.wait();  // get exclusive access to ThreadPool
  IdentifiedThreadPool &pool = instance.getTPool();
  for(size_t i = 0; i < jobs.size(); i++) {
    pool.fetchResource()->executeJob(*jobs[i], NULL);
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
  IdentifiedThreadPool &Tpool = instance.getTPool();

  CompactArray<IdentifiedThread*> threadArray(jobs.size());
  for(size_t i = 0; i < jobs.size(); i++) {
    threadArray.add(Tpool.fetchResource());
  }
  IdentifiedResultQueue *queue = instance.getQPool().fetchResource();
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

void ThreadPool::releaseThread(IdentifiedThread *thread) { // static
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

class PoolLogger : public Runnable {
  bool      m_stopped, m_killed, m_terminated;
  Semaphore m_start;
public:
  PoolLogger();
  void startLogging();
  void stopLogging();
  void killLogging();
  UINT run();
};

PoolLogger::PoolLogger() : m_start(0), m_stopped(true), m_killed(false), m_terminated(false) {
}

void PoolLogger::startLogging() {
  if(m_stopped) {
    m_stopped = false;
    m_start.notify();
  }
}

void PoolLogger::stopLogging() {
  m_stopped = true;
}

void PoolLogger::killLogging() {
  m_killed = true;
  startLogging();
}

UINT PoolLogger::run() {
  for(;;) {
    const int timeout = m_stopped ? INFINITE : 2000;
    m_start.wait(timeout);
    if(m_killed) break;
    if(m_stopped) {
      continue;
    }
    debugLog(_T("%s\n"), ThreadPool::getInstance().toString().cstr());
    if(m_killed) break;
  }
  m_terminated = true;
  return 0;
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
    ((PoolLogger*)instance.m_logger)->stopLogging();
  }
  instance.notify();
}

void ThreadPool::killLogging() { // static
  ThreadPool &instance = getInstance();
  instance.wait();
  if(instance.m_logger != NULL) {
    ((PoolLogger*)instance.m_logger)->killLogging();
  }
  instance.notify();
}
