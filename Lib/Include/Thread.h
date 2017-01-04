#pragma once

#include "MyUtil.h"
#include "Runnable.h"
#include "SynchronizedQueue.h"
#include "CompactStack.h"

class Thread;

HANDLE getCurrentThreadHandle(); // call DuplicateHandle(GetCurrentThread())
                                 // returned Handle should be closed with CloseHandle()
                                 // after use. Throw Exception on failure

class UncaughtExceptionHandler {
public:
  virtual void uncaughtException(Thread &thread, Exception &e) = 0;
};

class IdentifiedResource {
private:
  const int m_id;
public:
  IdentifiedResource(int id) : m_id(id) {
  }
  inline int getId() const {
    return m_id;
  }
};

/* Threadpriorities defined in winbase.h 
 *  THREAD_PRIORITY_IDLE            
 *  THREAD_PRIORITY_LOWEST          
 *  THREAD_PRIORITY_BELOW_NORMAL    
 *  THREAD_PRIORITY_NORMAL          
 *  THREAD_PRIORITY_ABOVE_NORMAL    
 *  THREAD_PRIORITY_TIME_CRITICAL   
 *  THREAD_PRIORITY_HIGHEST         
 */
class Thread : public Runnable {
private:
  DECLARECLASSNAME;
  friend UINT          threadStartup(Thread *thread);
  friend class         ThreadSet;
  friend class         InitThreadClass;

  static UncaughtExceptionHandler *s_defaultUncaughtExceptionHandler;
  String                    m_name;
  DWORD                     m_threadId;
  HANDLE                    m_threadHandle;
  Semaphore                 m_gate;
  Runnable                 *m_target;
  UncaughtExceptionHandler *m_uncaughtExceptionHandler;
  bool                      m_isDeamon;

  void handleUncaughtException(Exception &e);
  void init(Runnable *target, const String &name, size_t stackSize);
public:
  Thread(Runnable &target, const String &name = "", size_t stackSize = 0); // stacksize = 0, makes createThread use the default stacksize
  Thread(const String &name = "", size_t stackSize = 0);                   // do
  Thread(const Thread &src);                                               // Not defined. Thread not cloneable
  Thread &operator=(const Thread &src);                                    // Not defined. Thread not cloneable

  virtual ~Thread();
  void setPriority(int priority);
  int  getPriority() const;
  void setPriorityBoost(bool disablePriorityBoost);
  bool getPriorityBoost() const;
  bool stillActive() const;
  int  getThreadId() const {
    return m_threadId;
  }
  const String &getName() const {
    return m_name;
  }
  double getThreadTime(); // microseconds
  void setAffinityMask(DWORD mask);
  void setIdealProcessor(DWORD cpu);
  void suspend();
  void resume();
  void start() {
    resume();
  }

  UINT run();

  DWORD getId() const {
    return m_threadId;
  }
  void setDeamon(bool on) {
    m_isDeamon = on;
  }
  bool isDeamon() const {
    return m_isDeamon;
  }
  ULONG getExitCode() const;

  static void setDefaultUncaughtExceptionHandler(UncaughtExceptionHandler &eh) {
    s_defaultUncaughtExceptionHandler = &eh;
  }

  static UncaughtExceptionHandler &getDefaultUncaughtExceptionHandler() {
    return *s_defaultUncaughtExceptionHandler;
  }

  void setUncaughtExceptionHandler(UncaughtExceptionHandler &eh) {
    m_uncaughtExceptionHandler = &eh;
  }

  UncaughtExceptionHandler &getUncaughtExceptionHandler() {
    return *m_uncaughtExceptionHandler;
  }

  static EXECUTION_STATE setExecutionState(EXECUTION_STATE newState);

  static void keepAlive(EXECUTION_STATE flags = ES_CONTINUOUS | ES_SYSTEM_REQUIRED); // can add ES_DISPLAY_REQUIRED
};

class ThreadPoolResultQueue : public SynchronizedQueue<TCHAR*>, public IdentifiedResource {
private:
  ThreadPoolResultQueue(           const ThreadPoolResultQueue &src); // not implemented
  ThreadPoolResultQueue &operator=(const ThreadPoolResultQueue &src); // not implemented
public:
  ThreadPoolResultQueue(int id) : IdentifiedResource(id) {
  }
  void waitForResults(size_t expectedResultCount);
};

class ThreadPoolThread : public Thread, public IdentifiedResource {
private:
  Runnable               *m_job;
  ThreadPoolResultQueue  *m_resultQueue;
  Semaphore               m_execute;
  int                     m_requestCount;
public:
  ThreadPoolThread(int id);
  UINT run();
  void execute(Runnable &job, ThreadPoolResultQueue *resultQueue);
};

template <class T> class PoolIdentifiedResources : public CompactArray<T*> {
private:
  CompactStack<int> m_freeId;
protected:
  virtual void allocateNewResources(size_t count) {
    UINT id = (UINT)size();
    for(size_t i = 0; i < count; i++, id++) {
      m_freeId.push(id);
      add(new T(id));
    }
  }
public:
  PoolIdentifiedResources() {
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
      delete (*this)[i];
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
  String toString() const {
    const BitSet allocatedIdSet = getAllocatedIdSet();
    const BitSet freeIdSet      = getFreeIdSet();
    return format(_T("Free:%s. In use:%s")
                 ,freeIdSet.toString().cstr()
                 ,(allocatedIdSet - freeIdSet).toString().cstr()
                 );
  }
};

class PoolThreadPoolThreads : public PoolIdentifiedResources<ThreadPoolThread> {
private:
  int  m_threadPriority;
  bool m_disablePriorityBoost;

  Thread &get(UINT index) {
    return *((Thread*)((*this)[index]));
  }

protected:
  void allocateNewResources(size_t count) {
    const UINT oldSize = (UINT)size();
    PoolIdentifiedResources<ThreadPoolThread>::allocateNewResources(count);
    for(UINT i = oldSize; i < size(); i++) {
      Thread &thr = get(i);
      thr.setPriority(m_threadPriority);
      thr.setPriorityBoost(m_disablePriorityBoost);
    }
  }
public:
  PoolThreadPoolThreads() {
    m_threadPriority       = THREAD_PRIORITY_NORMAL;
    m_disablePriorityBoost = false;
  }
  void setPriority(int priority) {
    if(priority == m_threadPriority) {
      return;
    }
    m_threadPriority = priority;
    for(UINT i = 0; i < size(); i++) {
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
    for(UINT i = 0; i < size(); i++) {
      get(i).setPriorityBoost(disablePriorityBoost);
    }
  }
  bool getPriorityBoost() const {
    return m_disablePriorityBoost;
  }
};

typedef CompactArray<Runnable*> RunnableArray;

class ThreadPool {
  friend class ThreadPoolThread;
private:
  PoolThreadPoolThreads                          m_threadPool;
  PoolIdentifiedResources<ThreadPoolResultQueue> m_queuePool;
  mutable Semaphore                              m_gate;
  int                                            m_processorCount;
  int                                            m_activeThreads, m_maxActiveThreads;
  static ThreadPool                              s_instance;

  ThreadPool();
  ThreadPool(const ThreadPool &src);            // not implemented
  ThreadPool &operator=(const ThreadPool &src); // not implemented
  static void releaseThread(ThreadPoolThread *thr);
public:
  ~ThreadPool();
  static void executeNoWait(          Runnable &job);       // Execute job without blocking. Uncaught exceptions are lost.
  static void executeInParallelNoWait(RunnableArray &jobs); // execute all jobs without blocking. Uncaught exceptions are lost.
  static void executeInParallel(      RunnableArray &jobs); // Blocks until all jobs are done. If any of the jobs throws an exception
                                                            // the rest of the jobs will be terminated and an exception with the same
                                                            // message will be thrown to the caller.

  static int  getProcessorCount() {
    return getInstance().m_processorCount;
  }
  static inline int getMaxActiveThreads() {
    return getInstance().m_maxActiveThreads;
  }

  static String toString(); // for debug
  static void   startLogging();
  static void   stopLogging();
  static void   setPriority(int priority); // Sets the priority for all running and future running threads
  // Default is THREAD_PRIORITY_BELOW_NORMAL
  // THREAD_PRIORITY_IDLE,-PRIORITY_LOWEST,-PRIORITY_BELOW_NORMAL,-PRIORITY_NORMAL,-PRIORITY_ABOVE_NORMAL

  static void setPriorityBoost(bool disablePriorityBoost);
  static ThreadPool &getInstance();
};
