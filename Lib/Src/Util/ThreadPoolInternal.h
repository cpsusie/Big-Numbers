#pragma once

#include <ThreadPool.h>
#include <ResourcePoolTemplate.h>
#include <Thread.h>
#include <SynchronizedQueue.h>

#ifdef TRACE_THREADPOOL
#include <DebugLog.h>
#define TRACE(...) debugLog(__VA_ARGS__)
#else
#define TRACE(...)
#endif

class ThreadPoolResultQueue : private SynchronizedQueue<String*>, public IdentifiedResource {
private:
  ThreadPoolResultQueue(const ThreadPoolResultQueue &src);            // not implemented
  ThreadPoolResultQueue &operator=(const ThreadPoolResultQueue &src); // not implemented
  const String m_name;
public:
  ThreadPoolResultQueue(UINT id, const String &name) : IdentifiedResource(id), m_name(name) {
  }
  void waitForResults(size_t expectedResultCount);
  void putAllDone() {
    __super::put(NULL);
  }
  void putError(const String &s) {
    __super::put(new String(s));
  }
  const String &getName() const {
    return m_name;
  }
};

class ThreadPoolThread : public Thread, public IdentifiedResource {
private:
  PoolThreadPool         &m_pool;
  Runnable               *m_job;
  ThreadPoolResultQueue  *m_resultQueue;
  FastSemaphore           m_execute;
  int                     m_requestCount;
  std::atomic<BYTE>       m_flags;
public:
  ThreadPoolThread(PoolThreadPool *pool, UINT id, const String &name);
  ~ThreadPoolThread();
  UINT run();
  void executeJob(Runnable &job, ThreadPoolResultQueue *resultQueue);
  void requestTerminate();
};

class ResultQueuePool : public ResourcePoolTemplate<ThreadPoolResultQueue> {
protected:
  ThreadPoolResultQueue *newResource(UINT id) {
    ThreadPoolResultQueue *q = new ThreadPoolResultQueue(id, format(_T("%s(%u)"), getTypeName().cstr(), id)); TRACE_NEW(q);
    return q;
  }
public:
  ResultQueuePool() : ResourcePoolTemplate<ThreadPoolResultQueue>("PoolResultQueue") {
  }
};

class PoolThreadPool : public ResourcePoolTemplate<ThreadPoolThread> {
  friend class ThreadPoolThread;
private:
  int               m_threadPriority;
  bool              m_disablePriorityBoost;
  FastSemaphore     m_lock, m_activeIs0;
  int               m_activeCount;
  Thread &get(size_t index) {
    return *((Thread*)((*this)[index]));
  }
  inline void incrActiveCount() {
    m_lock.wait();
    if (m_activeCount++ == 0) m_activeIs0.wait();
    m_lock.notify();
  }
  inline void decrActiveCount() {
    m_lock.wait();
    if (--m_activeCount == 0) m_activeIs0.notify();
    m_lock.notify();
  }

protected:
  ThreadPoolThread *newResource(UINT id) {
    ThreadPoolThread *t = new ThreadPoolThread(this, id, format(_T("%s(%u)"), getTypeName().cstr(), id)); TRACE_NEW(t);
    return t;
  }

  void allocateNewResources(UINT count) {
    const size_t oldSize = size();
    __super::allocateNewResources(count);
    for(size_t i = oldSize; i < size(); i++) {
      Thread &thr = get(i);
      thr.setPriority(m_threadPriority);
      thr.setPriorityBoost(m_disablePriorityBoost);
    }
  }
public:
  PoolThreadPool() : ResourcePoolTemplate<ThreadPoolThread>("PoolThread"), m_activeCount(0) {
    m_threadPriority       = THREAD_PRIORITY_NORMAL;
    m_disablePriorityBoost = false;
  }
  ~PoolThreadPool() {
    TRACE(_T("%s called. activeCount=%d wait until 0\n"), __TFUNCTION__, m_activeCount);
    m_activeIs0.wait();
    TRACE(_T("%s passed zero check. activeCount=%d. Now deleting threads\n"), __TFUNCTION__, m_activeCount);
    deleteAll();
    TRACE(_T("%s done\n"), __TFUNCTION__);
  }
  void requestTerminateAll() {
    TRACE(_T("%s called\n"), __TFUNCTION__);
    for(size_t i = 0; i < size(); i++) {
      (*this)[i]->requestTerminate();
    }
    TRACE(_T("%s done\n"), __TFUNCTION__);
  }
  void setPriority(int priority) {
    if(priority == m_threadPriority) {
      return;
    }
    m_threadPriority = priority;
    for(size_t i = 0; i < size(); i++) {
      get(i).setPriority(priority);
    }
  }
  int getPriority() const {
    return m_threadPriority;
  }
  void setPriorityBoost(bool disablePriorityBoost) {
    if(disablePriorityBoost == m_disablePriorityBoost) {
      return;
    }
    m_disablePriorityBoost = disablePriorityBoost;
    for(size_t i = 0; i < size(); i++) {
      get(i).setPriorityBoost(disablePriorityBoost);
    }
  }
  bool getPriorityBoost() const {
    return m_disablePriorityBoost;
  }
};
