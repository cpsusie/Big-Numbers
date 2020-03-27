#pragma once

#include "MyUtil.h"
#include "Runnable.h"
#include "FastSemaphore.h"
#include "PropertyContainer.h"

//#define TRACE_THREAD

#ifdef TRACE_THREAD
#define TRACE_THREADPOOL
#endif // TRACE_THREAD

class Thread;

HANDLE getCurrentThreadHandle(); // call DuplicateHandle(GetCurrentThread())
                                 // returned Handle should be closed with CloseHandle()
                                 // after use. Throw Exception on failure

// if hThread is not specified, currentThreadHandle is used
void   setThreadDescription(const String &description, HANDLE hThread = INVALID_HANDLE_VALUE);
// if hThread is not specified, currentThreadHandle is used
String getThreadDescription(HANDLE hThread = INVALID_HANDLE_VALUE);

#ifdef _DEBUG
#define SETTHREADDESCRIPTION(description, ...) setThreadDescription(description, __VA_ARGS__)
#else
#define SETTHREADDESCRIPTION(description, ...)
#endif // _DEBUG

// microseconds. if thread  == INVALID_HANDLE_VALUE, return time for current Thread
double getThreadTime(HANDLE hThread = INVALID_HANDLE_VALUE);

class UncaughtExceptionHandler {
public:
  virtual void uncaughtException(Thread &thread, Exception &e) = 0;
};

typedef enum {
  THR_BLOCKNEWTHREADS  // bool // Thread::remove/addListener is blocked and should NOT be called on this notification
 ,THR_THREADSRUNNING   // bool
} ThreadMapProperty;

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
  void setPriority(int priority);
  int  getPriority() const;
  void setPriorityBoost(bool disablePriorityBoost);
  bool getPriorityBoost() const;
  bool stillActive() const;
  inline DWORD getThreadId() const {
    return m_threadId;
  }
  void   setDescription(const String &description);
  String getDescription() const;
  double getThreadTime(); // microseconds
  void   setAffinityMask(DWORD mask);
  void   setIdealProcessor(DWORD cpu);
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
