#pragma once

#include "MyUtil.h"
#include "Runnable.h"
#include "ThreadBase.h"
#include "FastSemaphore.h"
#include "PropertyContainer.h"

//#define TRACE_THREAD

#if defined(TRACE_THREAD)
#define TRACE_THREADPOOL
#endif // TRACE_THREAD

class Thread;

typedef enum {
  THR_BLOCKNEWTHREADS  // bool // Thread::remove/addListener is blocked and should NOT be called on this notification
 ,THR_THREADSRUNNING   // bool
} ThreadMapProperty;


class UncaughtExceptionHandler {
public:
  virtual void uncaughtException(Thread &thread, Exception &e) = 0;
};

class Thread : public Runnable {
  friend class         ThreadMap;
  friend class         ThreadPool;
private:
  DECLARECLASSNAME;
  friend UINT          threadStartup(Thread *thread);
  static PropertyContainer        *s_propertySource;
  static UncaughtExceptionHandler *s_defaultUncaughtExceptionHandler;
  static UINT                      s_activeCount;
  ThreadMap                       &m_map;
  DWORD                            m_threadId;
  HANDLE                           m_threadHandle;
  FastSemaphore                    m_terminated;
  Runnable                        *m_target;
  UncaughtExceptionHandler        *m_uncaughtExceptionHandler;
  bool                             m_isDemon;

  Thread(           const Thread &src);                                      // Not defined. Class not cloneable
  Thread &operator=(const Thread &src);                                      // Not defined. Class not cloneable

  void              handleUncaughtException(Exception &e);
  void              init(const String &desciption, Runnable *target, size_t stackSize);
  static ThreadMap &getMap();
public:
  Thread(const String &description, Runnable &target, size_t stackSize = 0); // stacksize = 0, makes createThread use the default stacksize
  Thread(const String &description                  , size_t stackSize = 0); // do
  virtual ~Thread();
  inline void setPriority(ThreadPriority priority) {
    setThreadPriority(priority, m_threadHandle);
  }
  inline ThreadPriority getPriority() const {
    return getThreadPriority(m_threadHandle);
  }
  inline void setPriorityBoost(bool disablePriorityBoost) {
    setThreadPriorityBoost(disablePriorityBoost, m_threadHandle);
  }
  inline bool getPriorityBoost() const {
    return getThreadPriorityBoost(m_threadHandle);
  }
  inline bool stillActive() const {
    return getExitCode() == STILL_ACTIVE;
  }
  inline DWORD getThreadId() const {
    return m_threadId;
  }
  inline void setDescription(const String &description) {
    setThreadDescription(description, m_threadHandle);
  }
  inline String getDescription() const {
    return getThreadDescription(m_threadHandle);
  }
   // microseconds
  inline double getThreadTime() const {
    return ::getThreadTime(m_threadHandle);
  }

  inline void setAffinityMask(DWORD mask) {
    setThreadAffinityMask(mask, m_threadHandle);
  }
  inline void setIdealProcessor(DWORD cpu) {
    setThreadIdealProcessor(cpu, m_threadHandle);
  }
  void   suspend();
  void   resume();
  inline void start() {
    resume();
  }

  UINT run();

  void setDemon(bool on);
  inline bool isDemon() const {
    return m_isDemon;
  }
  inline ULONG getExitCode() const {
    return getThreadExitCode(m_threadHandle);
  }

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
  static UINT getActiveThreadCount() {
    return s_activeCount;
  }
  // This pointer will be source (PropertyContainer*) int PropertyChangeListener::handlePropertyChanged
  static inline bool isPropertyContainer(const PropertyContainer *source) {
    return source == s_propertySource;
  }
  static void addListener(   PropertyChangeListener *listener);
  static void removeListener(PropertyChangeListener *listener);

  static EXECUTION_STATE setExecutionState(EXECUTION_STATE newState);

  static Thread *getThreadById(DWORD threadId);
  static inline Thread *getCurrentThread() {
    return getThreadById(GetCurrentThreadId());
  }
  static void keepAlive(EXECUTION_STATE flags = ES_CONTINUOUS | ES_SYSTEM_REQUIRED); // can add ES_DISPLAY_REQUIRED
};
