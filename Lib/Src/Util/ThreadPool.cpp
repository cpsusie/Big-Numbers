#include "pch.h"
#include <CPUInfo.h>
#include <Thread.h>

ThreadPool::ThreadPool() {
  m_gate.wait();

  m_processorCount = getProcessorCount();
  m_activeThreads  = m_maxActiveThreads = 1;

  m_gate.signal();
}

ThreadPool::~ThreadPool() {
  m_gate.wait();

  m_queuePool.deleteAll();

  m_gate.signal();
}

void ThreadPool::setPriority(int priority) { // static
  ThreadPool &instance = getInstance();
  instance.m_gate.wait();
  try {
    if(priority != instance.m_threadPool.getPriority()) {
      instance.m_threadPool.setPriority(priority);
    }
  } catch(...) {
    instance.m_gate.signal();
    throw;
  }
  instance.m_gate.signal();
}

void ThreadPool::setPriorityBoost(bool disablePriorityBoost) { // static
  ThreadPool &instance = getInstance();
  instance.m_gate.wait();
  try {
    if(disablePriorityBoost != instance.m_threadPool.getPriorityBoost()) {
      instance.m_threadPool.setPriorityBoost(disablePriorityBoost);
    }
  } catch(...) {
    instance.m_gate.signal();
    throw;
  }
  instance.m_gate.signal();
}

void ThreadPool::executeNoWait(Runnable &job) {
  ThreadPool &instance = getInstance();
  instance.m_gate.wait();  // get exclusive access to ThreadPool
  instance.m_threadPool.fetchResource()->execute(job, NULL);
  instance.m_gate.signal(); // open gate for other threads
}

 // execute all jobs without blocking. Uncaught exceptions are lost.
void ThreadPool::executeInParallelNoWait(RunnableArray &jobs) { // static
  ThreadPool &instance = getInstance();
  instance.m_gate.wait();  // get exclusive access to ThreadPool
  for(size_t i = 0; i < jobs.size(); i++) {
    instance.m_threadPool.fetchResource()->execute(*jobs[i], NULL);
  }
  instance.m_gate.signal(); // open gate for other threads
}

// Blocks until all jobs are done. If any of the jobs throws an exception
// the rest of the jobs will be terminated and an exception with the same
// message will be thrown to the caller
void ThreadPool::executeInParallel(RunnableArray &jobs) { // static
  if(jobs.size() == 0) {
    return;
  }
  ThreadPool &instance = getInstance();
  instance.m_gate.wait();  // get exclusive access to ThreadPool

  CompactArray<ThreadPoolThread*> threadArray(jobs.size());
  for(size_t i = 0; i < jobs.size(); i++) {
    threadArray.add(instance.m_threadPool.fetchResource());
  }
  ThreadPoolResultQueue *queue = instance.m_queuePool.fetchResource();
  for(size_t i = 0; i < jobs.size(); i++) {
    threadArray[i]->execute(*jobs[i], queue);
  }
  instance.m_gate.signal(); // open gate for other threads

  try {
    queue->waitForResults(jobs.size());
    instance.m_gate.wait();
    instance.m_queuePool.releaseResource(queue);
    instance.m_gate.signal();
  } catch(...) {
    instance.m_gate.wait();
    instance.m_queuePool.releaseResource(queue);
    instance.m_gate.signal();
    throw;
  }
}

void ThreadPool::releaseThread(ThreadPoolThread *thread) { // static
  ThreadPool &instance = getInstance();
  instance.m_gate.wait();
  instance.m_threadPool.releaseResource(thread);
  instance.m_gate.signal();
}

String ThreadPool::toString() { // static
  ThreadPool &instance = getInstance();
  String result;
  instance.m_gate.wait();

  result = format(_T("Threads:%s Queues:%s")
                 ,instance.m_threadPool.toString().cstr()
                 ,instance.m_queuePool.toString().cstr()
                 );
  instance.m_gate.signal();
  return result;
}

ThreadPool ThreadPool::s_instance;

ThreadPool &ThreadPool::getInstance() {
  return s_instance;
}

class PoolLoggingThread : public Thread {
  bool      m_stopped;
  Semaphore m_start;
public:
  PoolLoggingThread();
  void startLogging();
  void stopLogging();
  UINT run();
};

PoolLoggingThread::PoolLoggingThread() : m_start(0) {
  setDeamon(true);
  m_stopped = true;
  start();
}

void PoolLoggingThread::startLogging() {
  if(m_stopped) {
    m_stopped = false;
    m_start.signal();
  }
}

void PoolLoggingThread::stopLogging() {
  m_stopped = true;
}

UINT PoolLoggingThread::run() {
  for(;;) {
    const int timeout = m_stopped ? INFINITE : 2000;
    m_start.wait(timeout);
    if(m_stopped) {
      continue;
    }
    debugLog(_T("%s\n"), ThreadPool::getInstance().toString().cstr());
  }
}

static PoolLoggingThread loggingThread;

void ThreadPool::startLogging() {
  loggingThread.startLogging();
}

void ThreadPool::stopLogging() {
  loggingThread.stopLogging();
}
