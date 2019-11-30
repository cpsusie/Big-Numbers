#pragma once

#include <ThreadPool.h>
#include <Thread.h>
#include <SynchronizedQueue.h>

template <class T> class IdentifiedResourcePool : public CompactArray<T*> {
private:
  CompactStack<int> m_freeId;
protected:
  virtual T   *newResource(UINT id) = NULL;

  virtual void allocateNewResources(size_t count) {
    UINT id = (UINT)size();
    for(size_t i = 0; i < count; i++, id++) {
      m_freeId.push(id);
      T *r = newResource(id);
      add(r);
    }
  }
public:
  virtual ~IdentifiedResourcePool() {
    deleteAll();
  }
  T *fetchResource() {
    if(m_freeId.isEmpty()) {
      allocateNewResources(5);
    }
    const int index = m_freeId.pop();
    return (*this)[index];
  }

  void releaseResource(const IdentifiedResource *resource) {
    m_freeId.push(resource->getId());
  }

  void deleteAll() {
    for(size_t i = 0; i < size(); i++) {
      T *r = (*this)[i];
      SAFEDELETE(r);
    }
    clear();
    m_freeId.clear();
  }
  BitSet getAllocatedIdSet() const {
    if(size() == 0) {
      return BitSet(8);
    } else {
      BitSet result(size());
      return result.invert();
    }
  }
  BitSet getFreeIdSet() const {
    const int n = m_freeId.getHeight();
    if(n == 0) {
      return BitSet(8);
    } else {
      BitSet result(size());
      for(int i = 0; i < n; i++) {
        result.add(m_freeId.top(i));
      }
      return result;
    }
  }
  BitSet getActiveIdSet() const {
    return getAllocatedIdSet() - getFreeIdSet();
  }

  String toString() const {
    const BitSet allocatedIdSet = getAllocatedIdSet();
    const BitSet freeIdSet = getFreeIdSet();
    return format(_T("Free:%s. In use:%s")
                 ,freeIdSet.toString().cstr()
                 ,(allocatedIdSet - freeIdSet).toString().cstr()
    );
  }
};

class IdentifiedResultQueue : private SynchronizedQueue<String*>, public IdentifiedResource {
private:
  IdentifiedResultQueue(const IdentifiedResultQueue &src);            // not implemented
  IdentifiedResultQueue &operator=(const IdentifiedResultQueue &src); // not implemented

public:
  IdentifiedResultQueue(int id) : IdentifiedResource(id) {
  }
  void waitForResults(size_t expectedResultCount);
  void putAllDone() {
    __super::put(NULL);
  }
  void putError(const String &s) {
    __super::put(new String(s));
  }
};

class IdentifiedResultQueuePool : public IdentifiedResourcePool<IdentifiedResultQueue> {
protected:
  IdentifiedResultQueue   *newResource(UINT id) {
    IdentifiedResultQueue *q = new IdentifiedResultQueue(id); TRACE_NEW(q);
    return q;
  }
};

class IdentifiedThread : public Thread, public IdentifiedResource {
private:
  IdentifiedThreadPool   &m_pool;
  Runnable               *m_job;
  IdentifiedResultQueue  *m_resultQueue;
  FastSemaphore           m_execute;
  int                     m_requestCount;
  bool                    m_requestTerminate;
  bool                    m_busy;
public:
  IdentifiedThread(IdentifiedThreadPool *pool, int id);
  ~IdentifiedThread();
  UINT run();
  void executeJob(Runnable &job, IdentifiedResultQueue *resultQueue);
  inline bool isBusy() const {
    return m_busy;
  }
  void requestTerminate();
};

class IdentifiedThreadPool : public IdentifiedResourcePool<IdentifiedThread> {
  friend class IdentifiedThread;
private:
  int               m_threadPriority;
  bool              m_disablePriorityBoost;
  FastSemaphore     m_allTerminated;
  std::atomic<UINT> m_terminatedThreads;

  Thread &get(size_t index) {
    return *((Thread*)((*this)[index]));
  }
  void incrTerminatedThreads() {
    m_terminatedThreads++;
    if(m_terminatedThreads == size()) {
      m_allTerminated.notify();
    }
  }
protected:
  IdentifiedThread *newResource(UINT id) {
    IdentifiedThread *t = new IdentifiedThread(this, id); TRACE_NEW(t);
    return t;
  }

  void allocateNewResources(size_t count) {
    const size_t oldSize = size();
    __super::allocateNewResources(count);
    for(size_t i = oldSize; i < size(); i++) {
      Thread &thr = get(i);
      thr.setPriority(m_threadPriority);
      thr.setPriorityBoost(m_disablePriorityBoost);
    }
  }
public:
  IdentifiedThreadPool() : m_allTerminated(0) {
    m_threadPriority       = THREAD_PRIORITY_NORMAL;
    m_disablePriorityBoost = false;
  }
  ~IdentifiedThreadPool() {
    if(size()) {
      requestTerminateAll();
      m_allTerminated.wait();
    }
  }
  void requestTerminateAll() {
    for(size_t i = 0; i < size(); i++) {
      (*this)[i]->requestTerminate();
    }
  }
  void deleteAll() {
  }
  void setPriority(int priority) {
    if(priority == m_threadPriority) {
      return;
    }
    m_threadPriority = priority;
    for (size_t i = 0; i < size(); i++) {
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
