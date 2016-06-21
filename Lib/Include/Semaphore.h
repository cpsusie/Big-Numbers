#pragma once

#include "MyUtil.h"

class Semaphore {
private:
  DECLARECLASSNAME;
  HANDLE m_sem;
  DWORD  m_lockingThreadId;
  size_t m_enterCount;
public:
  // initialCount is the number of threads allowed in critical section
  // maxWait is maximal number of threads, waiting on this semaphore
  Semaphore(int initialCount=1, int maxWait=1000000);  // initialCount = [0..1]
  Semaphore(const Semaphore &src);            // not defined. Semaphore not cloneable
  Semaphore &operator=(const Semaphore &src); // do
  ~Semaphore();
  bool wait(int milliseconds=INFINITE); // return false on timeout, true if signaled, throws Exception on error
  void signal();
  void wait(  const char *name, const char *file, int line);
  void signal(const char *name, const char *file, int line);

};

#ifdef TRACESEMAPHORE

#define WAIT(sem)   sem.wait(  #sem, __FILE__, __LINE__)
#define SIGNAL(sem) sem.signal(#sem, __FILE__, __LINE__)

#else

#define WAIT(sem)   sem.wait()
#define SIGNAL(sem) sem.signal()

#endif
