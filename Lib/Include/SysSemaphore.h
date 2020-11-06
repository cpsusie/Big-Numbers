#pragma once

//#define TRACESEMAPHORE

class SysSemaphore {
private:
  HANDLE m_sem;
  SysSemaphore(           const SysSemaphore &src); // Not defined. Class not cloneable
  SysSemaphore &operator=(const SysSemaphore &src); // Not defined. Class not cloneable

public:
  // initialCount is the number of threads allowed in critical section (=initial value of internal counter)
  // maxWait is the max number of threads, allowed to wait on this semaphore
  // Assume 0 <= initialCount <= maxwait
  // The state of the semaphore is signaled when counter > 0, nonsignaled when counter == 0
  // Counter is decreased by 1 whenever a call to wait returns true (not if wait returns false because of timeout)
  // Counter is increased by 1 when notify is called (typically when a thread leaves a critical section, or a memberfunction of a monitor))
  SysSemaphore(const String &name, UINT initialCount = 1, UINT maxWait = 1000000);
  SysSemaphore(                    UINT initialCount = 1, UINT maxWait = 1000000);
  ~SysSemaphore();
  // Return false on timeout, true if state is signaled, throws Exception on error
  // timeout is in milliseconds
  // If timeout = INFINITE, wait will never return false, but wait until state is signaled
  bool wait(UINT timeout=INFINITE);
  void notify();
  HANDLE getHandle() const {
    return m_sem;
  }
#if defined(TRACESEMAPHORE)
  bool wait(  const TCHAR *name, const TCHAR *file, int line, const TCHAR *function, UINT timeout = INFINITE);
  void notify(const TCHAR *name, const TCHAR *file, int line, const TCHAR *function);
#endif  // TRACESEMAPHORE
};

#if defined(TRACESEMAPHORE)
#define WAIT(  sem,...) (sem).wait(  _T(#sem), __TFILE__, __LINE__, __TFUNCTION__, __VA_ARGS___)
#define NOTIFY(sem)     (sem).notify(_T(#sem), __TFILE__, __LINE__, __TFUNCTION__)
#else
#define WAIT(  sem,...) (sem).wait(__VA_ARGS__)
#define NOTIFY(sem)     (sem).notify()
#endif // TRACESEMAPHORE
