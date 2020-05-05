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
  THREADPOOL_SHUTTINGDDOWN
} ThreadPoolProperty;

class ThreadPool : public Singleton, public PropertyChangeListener, PropertyContainer {
  friend class SingletonFactory;
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
  void prepareDelete();
  void killLogger();
  ThreadPool(SingletonFactory *factory);
  ~ThreadPool();
  ThreadPool(const ThreadPool &src);            // not implemented
  ThreadPool &operator=(const ThreadPool &src); // not implemented
  void releaseThread(ThreadPoolThread *thr);
  inline PoolThreadPool &getTPool() const {
    return *m_threadPool;
  }
  inline ResultQueuePool &getQPool() const {
    return *m_queuePool;
  }
  inline void wait()   const { m_gate.wait();   }
  inline void notify() const { m_gate.notify(); }
public:
  static void executeNoWait(          Runnable      &job);  // Execute job without blocking. Uncaught exceptions are lost.
  static void executeInParallelNoWait(RunnableArray &jobs); // Execute all jobs without blocking. Uncaught exceptions are lost.
  static void executeInParallel(      RunnableArray &jobs); // Blocks until all jobs are done. If any of the jobs throws an exception
                                                            // the rest of the jobs will be terminated and an exception with the same
                                                            // message will be thrown to the caller.

  static inline int getMaxActiveThreads() {
    return getInstance().m_maxActiveThreads;
  }

  String toString() const; // for debug
  void   startLogging();
  void   stopLogging();
  static void   setPriority(ThreadPriority priority); // Sets the priority for all running and future running threads
  // Default is THREAD_PRIORITY_BELOW_NORMAL
  // THREAD_PRIORITY_IDLE,-PRIORITY_LOWEST,-PRIORITY_BELOW_NORMAL,-PRIORITY_NORMAL,-PRIORITY_ABOVE_NORMAL

  static void setPriorityBoost(bool disablePriorityBoost);
  static ThreadPool &getInstance();

  // This pointer will be source (PropertyContainer*) int PropertyChangeListener::handlePropertyChanged
  static inline bool isPropertyContainer(const PropertyContainer *source) {
    return source == s_propertySource;
  }

  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
  static void addListener(   PropertyChangeListener *listener);
  static void removeListener(PropertyChangeListener *listener);
};
