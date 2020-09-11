#pragma once

#include "BitSet.h"
#include "Runnable.h"
#include "Singleton.h"
#include "FastSemaphore.h"
#include "PropertyContainer.h"
#include "CompactStack.h"
#include "ThreadBase.h"

class PoolThreadPool;
class ResultQueuePool;
class PoolLogger;

typedef enum {
  THREADPOOL_SHUTTINGDDOWN   // bool
} ThreadPoolProperty;

class ThreadPool : public Singleton, public PropertyChangeListener, PropertyContainer {
  friend class ThreadPoolThread;
private:
  PoolThreadPool             *m_threadPool;
  ResultQueuePool            *m_queuePool;
  PoolLogger                 *m_logger;
  mutable FastSemaphore       m_gate;
  const int                   m_processorCount;
  int                         m_activeThreads, m_maxActiveThreads;
  bool                        m_blockExecute;
  static PropertyContainer   *s_propertySource;
  // private ..... no lock
  ThreadPool &ExecuteNoWait(Runnable &job);  // Execute job without blocking. Uncaught exceptions are lost.
  // no lock
  ThreadPool &ExecuteInParallelNoWait(RunnableArray &jobs);
  // no lock
  ThreadPool &PrepareDelete();
  // no lock
  ThreadPool &ReleaseThread(ThreadPoolThread *thr);
  // no lock
  ThreadPool &KillLogger();
  // no lock
  ThreadPool &AddListener(PropertyChangeListener *listener);
  // no lock
  ThreadPool &RemoveListener(PropertyChangeListener *listener);

  inline PoolThreadPool &GetTPool() const {
    return *m_threadPool;
  }
  // no lock
  inline ResultQueuePool &GetQPool() const {
    return *m_queuePool;
  }

  ThreadPool();
  ~ThreadPool() override;
  inline ThreadPool &wait()   { m_gate.wait();   return *this; }
  inline void        notify() { m_gate.notify(); }

  // with lock
  void prepareDelete();
  // with lock
  void killLogger();
  // with lock
  void releaseThread(ThreadPoolThread *thr);

public:
  static void executeNoWait(          Runnable      &job);  // Execute job without blocking. Uncaught exceptions are lost.
  static void executeInParallelNoWait(RunnableArray &jobs); // Execute all jobs without blocking. Uncaught exceptions are lost.
  static void executeInParallel(      RunnableArray &jobs); // Blocks until all jobs are done. If any of the jobs throws an exception
                                                            // the rest of the jobs will be terminated and an exception with the same
                                                            // message will be thrown to the caller.

  static inline int getMaxActiveThreads() {
    return getInstance().m_maxActiveThreads;
  }

  // with lock
  String toString() const; // for debug
  // with lock
  void   startLogging();
  // with lock
  void   stopLogging();
  static void   setPriority(ThreadPriority priority); // Sets the priority for all running and future running threads
  // Default is THREAD_PRIORITY_BELOW_NORMAL
  // THREAD_PRIORITY_IDLE,-PRIORITY_LOWEST,-PRIORITY_BELOW_NORMAL,-PRIORITY_NORMAL,-PRIORITY_ABOVE_NORMAL

  static void setPriorityBoost(bool disablePriorityBoost);
  // This pointer will be source (PropertyContainer*) int PropertyChangeListener::handlePropertyChanged
  static inline bool isPropertyContainer(const PropertyContainer *source) {
    return source == s_propertySource;
  }

  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) override;
  static void addListener(   PropertyChangeListener *listener);
  static void removeListener(PropertyChangeListener *listener);
  DEFINESINGLETON(ThreadPool)
};
