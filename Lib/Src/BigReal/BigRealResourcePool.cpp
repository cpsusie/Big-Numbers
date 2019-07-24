#include "pch.h"
#include <DebugLog.h>
#include <CPUInfo.h>

BigRealResourcePool::BigRealResourcePool() {
  m_gate.wait();

  m_processorCount = getProcessorCount();
  CompactArray<DigitPool*> poolArray;
  for(int i = 0; i < 8; i++) {
    poolArray.add(m_digitPool.fetchResource());
  }
  for(size_t i = 0; i < poolArray.size(); i++) {
    m_digitPool.releaseResource(poolArray[i]);
  }
  m_activeThreads  = m_maxActiveThreads = 1;
  m_gate.signal();
}

BigRealResourcePool::~BigRealResourcePool() {
  m_gate.wait();

  m_MTThreadPool.deleteAll();
  m_queuePool.deleteAll();
  m_digitPool.deleteAll();

  m_gate.signal();
}

void MThreadArray::waitForAllResults() {
  if(size() == 0) {
    return;
  } else {
    (*this)[0]->getQueue().waitForResults((int)size());
  }
}

MThreadArray::~MThreadArray() {
  BigRealResourcePool::releaseMTThreadArray(*this);
}

MThreadArray &BigRealResourcePool::fetchMTThreadArray(MThreadArray &threads, int count) { // static
  BigRealResourcePool &instance = getInstance();
  instance.m_gate.wait();
  threads.clear(count);
  if(count > 0) {
    SynchronizedStringQueue *queue = instance.m_queuePool.fetchResource();

    for(int i = 0; i < count; i++) {
      MultiplierThread *thread = instance.m_MTThreadPool.fetchResource();
      thread->m_digitPool      = instance.m_digitPool.fetchResource();
      thread->m_resultQueue    = queue;
      threads.add(thread);
    }

    instance.m_activeThreads += count;
    if(instance.m_activeThreads > instance.m_maxActiveThreads) {
      instance.m_maxActiveThreads = instance.m_activeThreads;
    }
  }
  instance.m_gate.signal();

  return threads;
}

void BigRealResourcePool::releaseMTThreadArray(MThreadArray &threads) { // static
  BigRealResourcePool &instance = getInstance();

  instance.m_gate.wait();
  if(threads.size() > 0) {
    instance.m_queuePool.releaseResource(threads[0]->m_resultQueue);
    for(size_t i = 0; i < threads.size(); i++) {
      MultiplierThread *thread = threads[i];
      instance.m_digitPool.releaseResource(thread->m_digitPool);
      thread->m_digitPool   = NULL;
      thread->m_resultQueue = NULL;

      instance.m_MTThreadPool.releaseResource(thread);
    }
    instance.m_activeThreads -= (int)threads.size();
    threads.clear();
  }
  instance.m_gate.signal();
}

void BigRealResourcePool::setPriority(int priority) { // static
  BigRealResourcePool &instance = getInstance();

  instance.m_gate.wait();
  try {
    if(priority != instance.m_threadPool.getPriority()) {
      instance.m_threadPool.setPriority(priority);
    }
    if(priority != instance.m_MTThreadPool.getPriority()) {
      instance.m_MTThreadPool.setPriority(priority);
    }
  } catch(...) {
    instance.m_gate.signal();
    throw;
  }
  instance.m_gate.signal();
}

void BigRealResourcePool::setPriorityBoost(bool disablePriorityBoost) { // static
  BigRealResourcePool &instance = getInstance();

  instance.m_gate.wait();
  try {
    if(disablePriorityBoost != instance.m_threadPool.getPriorityBoost()) {
      instance.m_threadPool.setPriorityBoost(disablePriorityBoost);
    }
    if(disablePriorityBoost != instance.m_MTThreadPool.getPriorityBoost()) {
      instance.m_MTThreadPool.setPriorityBoost(disablePriorityBoost);
    }
  } catch(...) {
    instance.m_gate.signal();
    throw;
  }
  instance.m_gate.signal();
}

DigitPool *BigRealResourcePool::fetchDigitPool() { // static
  BigRealResourcePool &instance = getInstance();
  instance.m_gate.wait();
  MTDigitPoolType *pool = instance.m_digitPool.fetchResource();
#ifdef CHECKALLDIGITS_RELEASED
  pool->m_usedDigits = pool->getUsedDigitCount();
#endif
  instance.m_gate.signal();
  return pool;
}

void BigRealResourcePool::releaseDigitPool(DigitPool *pool) { // static
  BigRealResourcePool &instance = getInstance();
#ifdef CHECKALLDIGITS_RELEASED
  MTDigitPoolType *tmp     = (MTDigitPoolType*)pool;
  const UINT       newUsed = tmp->getUsedDigitCount();
  if(tmp->m_usedDigits != newUsed) {
    throwBigRealException(_T("releaseDigitPool(%d).Used digits(fetch):%lu, Used digits(release):%lu"), pool->getId(), tmp->m_usedDigits, newUsed);
  }
#endif
  instance.m_gate.wait();
  instance.m_digitPool.releaseResource(pool);
  instance.m_gate.signal();
}

  // Blocks until all jobs are done. If any of the jobs throws an exception
  // the rest of the jobs will be terminated and an exception with the same
  // message will be thrown to the caller
void BigRealResourcePool::executeInParallel(CompactArray<Runnable*> &jobs) { // static
  if(jobs.size() == 0) return;
  BigRealResourcePool &instance = getInstance();
  instance.m_gate.wait();  // get exclusive access to BigRealResourcePool
  CompactArray<BigRealThread*> threads(jobs.size());
  for(size_t i = 0; i < jobs.size(); i++) {
    threads.add(instance.m_threadPool.fetchResource());
  }
  SynchronizedStringQueue *queue = instance.m_queuePool.fetchResource();
  for(size_t i = 0; i < jobs.size(); i++) {
    threads[i]->execute(*jobs[i], *queue);
  }
  instance.m_gate.signal(); // open gate for other threads

  try {
    queue->waitForResults((int)jobs.size());
    instance.m_gate.wait();
      for(size_t i = 0; i < threads.size(); i++) instance.m_threadPool.releaseResource(threads[i]);
      instance.m_queuePool.releaseResource(queue);
    instance.m_gate.signal();
  } catch(...) {
    instance.m_gate.wait();
      for(size_t i = 0; i < threads.size(); i++) instance.m_threadPool.releaseResource(threads[i]);
      instance.m_queuePool.releaseResource(queue);
    instance.m_gate.signal();
    throw;
  }
}

String BigRealResourcePool::toString() { // static
  BigRealResourcePool &instance = getInstance();
  String result;
  instance.m_gate.wait();

  result = format(_T("Threads:%s MTThreads:%s DigitPools:%s Queues:%s")
                 ,instance.m_threadPool.toString().cstr()
                 ,instance.m_MTThreadPool.toString().cstr()
                 ,instance.m_digitPool.toString().cstr()
                 ,instance.m_queuePool.toString().cstr()
                 );
  instance.m_gate.signal();
  return result;
}

class BigRealResourcePoolCreator {
private:
  BigRealResourcePool *m_instance;
  Semaphore            m_gate;
public:
  BigRealResourcePoolCreator() : m_instance(NULL) {
  }
  ~BigRealResourcePoolCreator() {
    SAFEDELETE(m_instance);
  }
  BigRealResourcePool &getInstance() {
    m_gate.wait();
    if(m_instance == NULL) {
      m_instance = new BigRealResourcePool; TRACE_NEW(m_instance);
    }
    m_gate.signal();
    return *m_instance;
  }
};

BigRealResourcePool &BigRealResourcePool::getInstance() {
  static BigRealResourcePoolCreator creator;
  return creator.getInstance();
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
    debugLog(_T("%s\n"), BigRealResourcePool::getInstance().toString().cstr());
  }
}

static PoolLoggingThread loggingThread;

void BigRealResourcePool::startLogging() {
  loggingThread.startLogging();
}

void BigRealResourcePool::stopLogging() {
  loggingThread.stopLogging();
}
