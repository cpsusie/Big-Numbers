#include "pch.h"
#include <Process.h>
#include <processthreadsapi.h>
#include <MyUtil.h>
#include <Singleton.h>
#include <Thread.h>
#include <FastSemaphore.h>
#include <CompactHashMap.h>
#include <eh.h>

DEFINECLASSNAME(Thread);

#if defined(TRACE_THREAD)

void threadTrace(const TCHAR *function, const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  String msg = vformat(format, argptr);
  va_end(argptr);
  debugLog(_T("%-20s:%s\n"), function, msg.cstr());
}
#define THREAD_TRACE(format,...) threadTrace(__TFUNCTION__,_T(format), __VA_ARGS__)
#define THREAD_ENTER ENTERFUNC
#define THREAD_LEAVE LEAVEFUNC
#else
#define THREAD_TRACE(...)
#define THREAD_ENTER
#define THREAD_LEAVE
#endif

class DefaultExceptionHandler : public UncaughtExceptionHandler {
public:
  void uncaughtException(Thread &thread, Exception &e);
};

void DefaultExceptionHandler::uncaughtException(Thread &thread, Exception &e) {
  String errorText = format(_T("Uncaught Exception in thread %s(%lu)\n%s\n"), thread.getDescription().cstr(), thread.getThreadId(), e.what());
  if(isatty(stderr)) {
    _ftprintf(stderr, _T("\n%s\n"), errorText.cstr());
  } else {
    MessageBox(NULL, errorText.replace(_T('\n'), _T("\r\n")).cstr(), _T("Error"), MB_OK | MB_ICONERROR);
  }
  abort();
}

// map ThreadId -> Thread*
class ThreadMap : public Singleton, private CompactUIntHashMap<Thread*,256>, private PropertyContainer {
private:
  mutable FastSemaphore m_lock;
  mutable FastSemaphore m_activeCountLock, m_activeIsZero; // need separate locks to prevent deadlock
                                                           // blockNewThreads calls m_lock.wait()....causing threads to terminate
                                                           // that is, returning to threadStartup, where they will call updateActiveCount(-1)
                                                           // If this function uses m_lock, no thread will ever terminate, but wait forever
                                                           // until m_lock.notify() is called, which will not happen until m_activeCount = 0
  bool                  m_blockNewThreads; // set to true, when destructor is called, so no more threads wil be started
  bool                  m_threadsRunning;
  void killDemonThreads();
  inline void blockNewThreads() {
    m_lock.wait();
    THREAD_TRACE("send notification THR_BLOCKNEWTHREADS");
    setProperty(THR_BLOCKNEWTHREADS, m_blockNewThreads, true);
    m_lock.notify();
  }
  ThreadMap();
  ~ThreadMap() override;
public:
  // throws exception if destructor has been called (as part of terminating this process)
  // Return true, if thread was added
  bool    addThread(   Thread *thread);
  bool    removeThread(Thread *thread);
  Thread *findThread(DWORD threadId) const;
  void incrActiveCount();
  void decrActiveCount();
  void addListener(PropertyChangeListener *listener) {
    m_lock.wait();
    addPropertyChangeListener(listener);
    m_lock.notify();
  }
  void removeListener(PropertyChangeListener *listener) {
    m_lock.wait();
    removePropertyChangeListener(listener);
    m_lock.notify();
  }
  bool hasListener(PropertyChangeListener *listener) const {
    return __super::hasListener(listener);
  }
  bool isEmpty() const;
  DEFINESINGLETON(ThreadMap)
};

typedef Entry<CompactUIntKeyType, Thread*> ThreadMapEntry;

ThreadMap::ThreadMap() : Singleton(__TFUNCTION__), m_blockNewThreads(false) {
  THREAD_ENTER;
  Thread::s_propertySource = this;
  Thread::s_defaultUncaughtExceptionHandler = new DefaultExceptionHandler; TRACE_NEW(Thread::s_defaultUncaughtExceptionHandler);
  THREAD_LEAVE;
}

ThreadMap::~ThreadMap() {
  THREAD_ENTER;
  blockNewThreads();
  killDemonThreads();
  THREAD_TRACE("waiting for activeisZero");
  m_activeIsZero.wait();
  THREAD_TRACE("activeisZero passed");
  SAFEDELETE(Thread::s_defaultUncaughtExceptionHandler);
  THREAD_LEAVE;
}

void ThreadMap::incrActiveCount() {
  m_activeCountLock.wait();
  if(Thread::s_activeCount++ == 0) {
    m_activeIsZero.wait();
    THREAD_TRACE("send notification THR_THREADSRUNNING=true");
    setProperty(THR_THREADSRUNNING, m_threadsRunning, true);
  }
  m_activeCountLock.notify();
}

void ThreadMap::decrActiveCount() {
  m_activeCountLock.wait();
  if(--Thread::s_activeCount == 0) {
    THREAD_TRACE("send notification THR_THREADSRUNNING=false");
    setProperty(THR_THREADSRUNNING, m_threadsRunning, false);
    m_activeIsZero.notify();
  }
  m_activeCountLock.notify();
}

bool ThreadMap::addThread(Thread *thread) {
  m_lock.wait();
  if(m_blockNewThreads) {
    m_lock.notify();
    throwException("No more threads can be started. Program is exitting");
  }
  size_t count = size();
  const bool result = put(thread->getThreadId(), thread);
  THREAD_TRACE("(%u)(res=%s). size=%zu",thread->getThreadId(), boolToStr(result), size());
  m_lock.notify();
  return result;
}

bool ThreadMap::removeThread(Thread *thread) {
  m_lock.wait();
  size_t count = size();
  const bool result = remove(thread->getThreadId());
  THREAD_TRACE("(%u)(res=%s). size=%zu", thread->getThreadId(), boolToStr(result), size());
  m_lock.notify();
  return result;
}

Thread *ThreadMap::findThread(DWORD threadId) const {
  m_lock.wait();
  Thread **t = get(threadId);
  m_lock.notify();
  return t ? *t : NULL;
}

bool ThreadMap::isEmpty() const {
  m_lock.wait();
  const bool result = __super::isEmpty();
  m_lock.notify();
  return result;
}

void ThreadMap::killDemonThreads() {
  m_lock.wait();
  CompactArray<Thread*> demonArray;
  for(Iterator<ThreadMapEntry> it = getEntryIterator(); it.hasNext();) {
    Thread *t = it.next().getValue();
    if(t->isDemon()) {
      demonArray.add(t);
    }
  }
  int terminateCount = 0;
  for(Thread *thr : demonArray) {
    THREAD_TRACE("Terminating demonthread(%d)", thr->getThreadId());
    if(TerminateThread(thr->m_threadHandle, 0)) {
      decrActiveCount();
      terminateCount++;
    }
  }
  if(terminateCount != demonArray.size()) {
    THREAD_TRACE("Cannot terminate all demon threads");
  }
  m_lock.notify();
}

// ----------------------------------------------------------------------------------------------------------

static UINT threadStartup(Thread *thread) {
  UINT result = -1;
  try {
    _set_se_translator(exceptionTranslator);
    try {
      thread->m_map.incrActiveCount();
      thread->m_terminated.wait();
      result = thread->run();
      thread->m_terminated.notify();
      thread->m_map.decrActiveCount();
    } catch(Exception e) {
      thread->handleUncaughtException(e);
      throw;
    } catch(...) {
      thread->handleUncaughtException(Exception(_T("Unknown Exception")));
      throw;
    }
  } catch(...) {
    thread->m_terminated.notify();
    thread->m_map.decrActiveCount();
    throw;
  }
  return result;
}

PropertyContainer        *Thread::s_propertySource                  = NULL;
UncaughtExceptionHandler *Thread::s_defaultUncaughtExceptionHandler = NULL;
UINT                      Thread::s_activeCount                     = 0;

ThreadMap &Thread::getMap() { // static
  return ThreadMap::getInstance();
}

Thread::Thread(const String &description, Runnable &target, size_t stackSize)
: m_map(getMap())
{
  init(description, &target, stackSize);
}

Thread::Thread(const String &description, size_t stackSize)
: m_map(getMap())
{
  init(description, NULL, stackSize);
}

void Thread::init(const String &description, Runnable *target, size_t stackSize) {
  THREAD_ENTER;
  m_target                   = target;
  m_isDemon                  = false;
  m_uncaughtExceptionHandler = NULL;
  m_threadHandle             = CreateThread(NULL, stackSize,(LPTHREAD_START_ROUTINE)threadStartup, this, CREATE_SUSPENDED, &m_threadId);
  if(m_threadHandle == NULL) {
    throwMethodLastErrorOnSysCallException(s_className, _T("CreateThread"));
  }
  setDescription(description);
  THREAD_TRACE("Thread(%u) desc=%s, created", m_threadId, description.cstr());
  m_map.addThread(this);
  THREAD_LEAVE;
}

Thread::~Thread() {
  THREAD_ENTER;
  if(!m_isDemon) {
    m_terminated.wait();
  }
  m_map.removeThread(this);
  CloseHandle(m_threadHandle);
  THREAD_TRACE("Thread(%u) deleted", m_threadId);
  THREAD_LEAVE;
}

void Thread::handleUncaughtException(Exception &e) {
  if(m_uncaughtExceptionHandler != NULL) {
    m_uncaughtExceptionHandler->uncaughtException(*this, e);
  } else {
    s_defaultUncaughtExceptionHandler->uncaughtException(*this, e);
  }
}

void Thread::setDemon(bool on) {
  if(on) {
    int fisk = 1;
  }
  m_isDemon = on;
}

void Thread::suspend() {
  SuspendThread(m_threadHandle);
}

void Thread::resume() {
  ResumeThread(m_threadHandle);
}

UINT Thread::run() {
  return m_target ? m_target->run() : 0;
}

void Thread::addListener(PropertyChangeListener *listener) { // static
  getMap().addListener(listener);
}

void Thread::removeListener(PropertyChangeListener *listener) { // static
  getMap().removeListener(listener);
}

Thread *Thread::getThreadById(DWORD threadId) { // static
  return getMap().findThread(threadId);
}

EXECUTION_STATE Thread::setExecutionState(EXECUTION_STATE newState) { // static
  return SetThreadExecutionState(newState);
}
