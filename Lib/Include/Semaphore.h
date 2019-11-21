#pragma once

class Semaphore {
private:
  HANDLE m_sem;
  Semaphore(const Semaphore &src);            // Not defined. Class not cloneable
  Semaphore &operator=(const Semaphore &src); // Not defined. Class not cloneable

public:
  // initialCount is the number of threads allowed in critical section
  // maxWait is maximal number of threads, waiting on this semaphore
  Semaphore(int initialCount=1, int maxWait=1000000);
  ~Semaphore();
  bool wait(int milliseconds=INFINITE); // return false on timeout, true if signaled, throws Exception on error
  void notify();
  void wait(  const TCHAR *name, const TCHAR *file, int line);
  void notify(const TCHAR *name, const TCHAR *file, int line);
  HANDLE getHandle() const {
    return m_sem;
  }
};

#ifdef TRACESEMAPHORE

#define WAIT(  sem) sem.wait(  _T(#sem), __TFILE__, __LINE__)
#define NOTIFY(sem) sem.NOTIFY(_T(#sem), __TFILE__, __LINE__)

#else

#define WAIT(  sem) sem.wait()
#define NOTIFY(sem) sem.notify()

#endif // TRACESEMAPHORE
