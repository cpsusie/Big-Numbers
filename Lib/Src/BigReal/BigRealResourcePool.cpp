#include "pch.h"
#include "Factory.h"
#include <DebugLog.h>
#include <CPUInfo.h>

BigRealResourcePool::BigRealResourcePool()
: m_queuePool(_T("Queue"))
, m_digitPool(_T("DigitPool"))
, m_lockedDigitPool(_T("LockedDigitPool"))
{
  m_gate.wait();

  m_processorCount = getProcessorCount();
  CompactArray<DigitPool*> poolArray;
  for(UINT i = 0; i < 8               ; i++) poolArray.add(m_digitPool.fetchResource());
  for(UINT i = 0; i < poolArray.size(); i++) m_digitPool.releaseResource(poolArray[i]);
  poolArray.clear(-1);
  for (UINT i = 0; i < 3              ; i++) poolArray.add(m_lockedDigitPool.fetchResource());
  for (UINT i = 0; i < poolArray.size(); i++) m_lockedDigitPool.releaseResource(poolArray[i]);
  poolArray.clear(-1);

  m_activeThreads  = m_maxActiveThreads = 1;
  m_gate.notify();
}

BigRealResourcePool::~BigRealResourcePool() {
  m_gate.wait();

  m_MTThreadPool.deleteAll();
  m_queuePool.deleteAll();
  m_digitPool.deleteAll();
  m_lockedDigitPool.deleteAll();

  m_gate.notify();
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
  instance.m_gate.notify();

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
  instance.m_gate.notify();
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
    instance.m_gate.notify();
    throw;
  }
  instance.m_gate.notify();
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
    instance.m_gate.notify();
    throw;
  }
  instance.m_gate.notify();
}

DigitPool *BigRealResourcePool::fetchDigitPool(bool withLock, BYTE initFlags) { // static
  BigRealResourcePool &instance = getInstance();
  instance.m_gate.wait();

  MTDigitPoolType *pool = withLock
                        ? instance.m_lockedDigitPool.fetchResource()
                        : instance.m_digitPool.fetchResource();

  assert(pool->continueCalculation());
#ifdef CHECKALLDIGITS_RELEASED
  pool->m_usedDigits = pool->getUsedDigitCount();
#endif
  pool->setInitFlags(initFlags);
  pool->saveRefCount();
  instance.m_gate.notify();
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
  try {
    const UINT refCount = pool->getRefCount();
    if(refCount != pool->getRefCountOnFetch()) {
      throwException(_T("%s:DigitPool \"%s\" has refCount=%u on release, refCount=%u on fetch")
                    ,__TFUNCTION__
                    ,pool->getName().cstr()
                    ,refCount
                    ,pool->getRefCountOnFetch());
    }
    pool->resetPoolCalculation();

    if(pool->isWithLock()) {
      instance.m_lockedDigitPool.releaseResource(pool);
    } else {
      instance.m_digitPool.releaseResource(pool);
    }
    instance.m_gate.notify();
  } catch(...) {
    instance.m_gate.notify();
    throw;
  }
}

void BigRealResourcePool::terminateAllPoolCalculations() { // // static
  BigRealResourcePool &instance = getInstance();
  instance.m_gate.wait();

  Iterator<MTDigitPoolType*> it = instance.m_digitPool.getActiveIterator();
  while(it.hasNext()) {
    it.next()->terminatePoolCalculation();
  }
  instance.m_gate.notify();
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
  instance.m_gate.notify(); // open gate for other threads

  try {
    queue->waitForResults((int)jobs.size());
    instance.m_gate.wait();
      for(size_t i = 0; i < threads.size(); i++) instance.m_threadPool.releaseResource(threads[i]);
      instance.m_queuePool.releaseResource(queue);
    instance.m_gate.notify();
  } catch(...) {
    instance.m_gate.wait();
      for(size_t i = 0; i < threads.size(); i++) instance.m_threadPool.releaseResource(threads[i]);
      instance.m_queuePool.releaseResource(queue);
    instance.m_gate.notify();
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
  instance.m_gate.notify();
  return result;
}

BigRealResourcePool &BigRealResourcePool::getInstance() {
  static FactoryTemplate<BigRealResourcePool> factory;
  return factory.getInstance();
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
    m_start.notify();
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
