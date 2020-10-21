#pragma once

#include <ResourcePoolTemplate.h>
#include <Thread.h>
#include <ThreadPool.h>
#include <FlagTraits.h>
#include <SynchronizedQueue.h>

#if defined(TRACE_THREADPOOL)
void threadPoolTrace(const TCHAR *function, const TCHAR *format, ...);
#define THREADPOOL_TRACE(format,...) threadPoolTrace(__TFUNCTION__,_T(format), __VA_ARGS__)
#define THREADPOOL_ENTER ENTERFUNC
#define THREADPOOL_LEAVE LEAVEFUNC
#else
#define THREADPOOL_TRACE(...)
#define THREADPOOL_ENTER
#define THREADPOOL_LEAVE
#endif // TRACE_THREADPOOL

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
    __super::put(nullptr);
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
  ATOMICFLAGTRAITS(ThreadPoolThread, BYTE, m_flags)
public:
  ThreadPoolThread(PoolThreadPool *pool, UINT id, const String &name);
  ~ThreadPoolThread();
  UINT run();
  void executeJob(Runnable &job, ThreadPoolResultQueue *resultQueue);
  void requestTerminate();
};

class ResultQueuePool : public ResourcePoolTemplate<ThreadPoolResultQueue> {
private:
  ThreadPool &m_threadPool;
protected:
  ThreadPoolResultQueue *newResource(UINT id) {
    ThreadPoolResultQueue *q = new ThreadPoolResultQueue(id, format(_T("%s(%u)"), getTypeName().cstr(), id)); TRACE_NEW(q);
    return q;
  }
public:
  ResultQueuePool(ThreadPool *threadPool)
    : m_threadPool(*threadPool)
    , ResourcePoolTemplate<ThreadPoolResultQueue>("PoolResultQueue") {
  }
  inline ThreadPool &getThreadPool() const {
    return m_threadPool;
  }
};

class PoolThreadPool : public ResourcePoolTemplate<ThreadPoolThread> {
  friend class ThreadPoolThread;
private:
  ThreadPool       &m_threadPool;
  ThreadPriority    m_threadPriority;
  bool              m_disablePriorityBoost;
  FastSemaphore     m_lock, m_activeIs0;
  int               m_activeCount;
  Thread &get(size_t index) {
    return *((Thread*)((*this)[index]));
  }
  inline void incrActiveCount() {
    m_lock.wait();
    if(m_activeCount++ == 0) m_activeIs0.wait();
    m_lock.notify();
  }
  inline void decrActiveCount() {
    m_lock.wait();
    if(--m_activeCount == 0) m_activeIs0.notify();
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
  PoolThreadPool(ThreadPool *threadPool)
    : m_threadPool(*threadPool)
    , ResourcePoolTemplate<ThreadPoolThread>("PoolThread")
    , m_activeCount(0)
  {
    m_threadPriority       = PRIORITY_NORMAL;
    m_disablePriorityBoost = false;
  }
  ~PoolThreadPool() {
    THREADPOOL_TRACE("%s called. activeCount=%d wait until 0\n", __TFUNCTION__, m_activeCount);
    m_activeIs0.wait();
    THREADPOOL_TRACE("%s passed zero check. activeCount=%d. Now deleting threads\n", __TFUNCTION__, m_activeCount);
    deleteAll();
    THREADPOOL_TRACE("%s done\n", __TFUNCTION__);
  }
  inline ThreadPool &getThreadPool() const {
    return m_threadPool;
  }
  void requestTerminateAll() {
    THREADPOOL_ENTER;
    for(size_t i = 0; i < size(); i++) {
      (*this)[i]->requestTerminate();
    }
    THREADPOOL_LEAVE;
  }
  void setPriority(ThreadPriority priority) {
    if(priority == m_threadPriority) {
      return;
    }
    m_threadPriority = priority;
    for(size_t i = 0; i < size(); i++) {
      get(i).setPriority(priority);
    }
  }
  ThreadPriority getPriority() const {
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
