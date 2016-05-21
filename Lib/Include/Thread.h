#pragma once

#include "MyUtil.h"
#include "Runnable.h"
#include "Semaphore.h"

class Thread;

class UncaughtExceptionHandler {
public:
  virtual void uncaughtException(Thread &thread, Exception &e) = 0;
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

  static UncaughtExceptionHandler *defaultUncaughtExceptionHandler;
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

  unsigned int run();

  unsigned long getId() const {
    return m_threadId;
  }
  void setDeamon(bool on) {
    m_isDeamon = on;
  }
  bool isDeamon() const {
    return m_isDeamon;
  }
  unsigned long getExitCode() const;

  static void setDefaultUncaughtExceptionHandler(UncaughtExceptionHandler &eh) {
    defaultUncaughtExceptionHandler = &eh;
  }

  static UncaughtExceptionHandler &getDefaultUncaughtExceptionHandler() {
    return *defaultUncaughtExceptionHandler;
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

