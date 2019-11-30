#include "pch.h"
#include <Process.h>
#include <processthreadsapi.h>
#include <MyUtil.h>
#include <SingletonFactory.h>
#include <Thread.h>
#include <FastSemaphore.h>
#include <CompactHashMap.h>
#include <eh.h>

DEFINECLASSNAME(Thread);

class DefaultExceptionHandler : public UncaughtExceptionHandler {
public:
  void uncaughtException(Thread &thread, Exception &e);
};

void DefaultExceptionHandler::uncaughtException(Thread &thread, Exception &e) {
  String errorText = format(_T("Uncaught Exception in thread %s(%lu)\n%s\n"), thread.getDescription().cstr(), thread.getId(), e.what());
  if(isatty(stderr)) {
    _ftprintf(stderr, _T("\n%s\n"), errorText.cstr());
  } else {
    MessageBox(NULL, errorText.replace(_T('\n'), _T("\r\n")).cstr(), _T("Error"), MB_OK | MB_ICONERROR);
  }
  abort();
}

// map ThreadId -> Thread*
class ThreadMap : private CompactUIntHashMap<Thread*>, private PropertyContainer {
  friend class ThreadMapFactory;

private:
  mutable FastSemaphore m_lock, m_listenerLock, m_propertyLock, m_activeCountLock, m_activeIsZero;
  UINT                  m_activeCount;
  bool                  m_blockNewThreads; // set to true, when destructor is called, so no more threads wil be started
  void killDemonThreads();
  inline void blockNewThreads() {
    m_propertyLock.wait();
    setProperty(THR_SHUTTINGDDOWN, m_blockNewThreads, true);
    m_propertyLock.notify();
  }

  ThreadMap();
  ~ThreadMap();                               // declared virtual in Collection
  ThreadMap(const ThreadMap &src);            // not implemented
  ThreadMap &operator=(const ThreadMap &src); // not implemented

public:
  // throws exception if destructor has been called (as part of terminating this process)
  // Return true, if thread was added
  bool    addThread(   Thread *thread);
  bool    removeThread(Thread *thread);
  Thread *findThread(DWORD threadId) const;
  inline void updateActiveCount(int n) {
    m_activeCountLock.wait();
    const bool was0 = m_activeCount == 0;
    m_activeCount += n;
    const bool is0 = m_activeCount == 0;
    if(was0 != is0) {
      if(n > 0) m_activeIsZero.wait(); else m_activeIsZero.notify();
    }
    m_activeCountLock.notify();
  }
  void addListener(PropertyChangeListener *listener) {
    m_listenerLock.wait();
    addPropertyChangeListener(listener);
    m_listenerLock.notify();
  }
  void removeListener(PropertyChangeListener *listener) {
    m_listenerLock.wait();
    removePropertyChangeListener(listener);
    m_listenerLock.notify();
  }
  bool isEmpty() const;
  UINT getActiveCount() const;
};

typedef Entry<CompactUIntKeyType, Thread*> ThreadMapEntry;

ThreadMap::ThreadMap() : m_blockNewThreads(false), m_activeCount(0) {
  Thread::s_propertySource = this;
  Thread::s_defaultUncaughtExceptionHandler = new DefaultExceptionHandler; TRACE_NEW(Thread::s_defaultUncaughtExceptionHandler);
}

ThreadMap::~ThreadMap() { // declared virtual in Collection
  blockNewThreads();
  killDemonThreads();
  m_activeIsZero.wait();
  SAFEDELETE(Thread::s_defaultUncaughtExceptionHandler);
  Thread::s_propertySource = NULL;
}

bool ThreadMap::addThread(Thread *thread) {
  m_lock.wait();
  if(m_blockNewThreads) {
    m_lock.notify();
    throwException("No more threads can be started. Program is exitting");
  }
  size_t count = size();
  const bool result = put(thread->m_threadId, thread);
  m_lock.notify();
  return result;
}

bool ThreadMap::removeThread(Thread *thread) {
  m_lock.wait();
  size_t count = size();
  const bool result = remove(thread->getId());
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
  const size_t n = demonArray.size();
  for(size_t i = 0; i < n; i++) {
    Thread *t = demonArray[i];
    if(TerminateThread(t->m_threadHandle, 0)) {
      updateActiveCount(-1);
      terminateCount++;
    }
  }
  if(terminateCount != n) {
    debugLog(_T("Cannot terminate all demon threads\n"));
  }
  m_lock.notify();
}

UINT ThreadMap::getActiveCount() const {
  m_lock.wait();
  UINT count = 0;
  for(Iterator<ThreadMapEntry> it = getEntryIterator(); it.hasNext();) {
    Thread *t = it.next().getValue();
    if(t->stillActive()) {
      count++;
    }
  }
  if(count != m_activeCount) {
    int fisk = 1;
  }
  m_lock.notify();
  return count;
}

// ----------------------------------------------------------------------------------------------------------

static UINT threadStartup(Thread *thread) {
  UINT result = -1;
  try {
    _set_se_translator(exceptionTranslator);
    try {
      Thread::getMap().updateActiveCount(1);
      result = thread->run();
      Thread::getMap().updateActiveCount(-1);
      thread->m_terminated.notify();
    } catch(Exception e) {
      thread->handleUncaughtException(e);
      throw;
    } catch(...) {
      thread->handleUncaughtException(Exception(_T("Unknown Exception")));
      throw;
    }
  } catch(...) {
    Thread::getMap().updateActiveCount(-1);
    thread->m_terminated.notify();
    throw;
  }
  return result;
}

PropertyContainer        *Thread::s_propertySource                  = NULL;
UncaughtExceptionHandler *Thread::s_defaultUncaughtExceptionHandler = NULL;

typedef enum {
  REQUEST_GETINSTANCE
 ,REQUEST_RELEASE
} POOLREQUEST;

DEFINESINGLETONFACTORY(ThreadMap);

ThreadMap *Thread::threadMapRequest(int request) {
  static ThreadMapFactory factory;
  switch(request) {
  case REQUEST_GETINSTANCE:
    return &factory.getInstance();
  case REQUEST_RELEASE:
    factory.releaseInstance();
    break;
  }
  return NULL;
}

ThreadMap &Thread::getMap() { // static
  return *threadMapRequest(REQUEST_GETINSTANCE);
}

void Thread::releaseMap() { // static
  threadMapRequest(REQUEST_RELEASE);
}

Thread::Thread(const String &description, Runnable &target, size_t stackSize) : m_terminated(0) {
  init(description, &target, stackSize);
}

Thread::Thread(const String &description, size_t stackSize) : m_terminated(0) {
  init(description, NULL, stackSize);
}

void Thread::init(const String &description, Runnable *target, size_t stackSize) {
  m_target                   = target;
  m_isDemon                 = false;
  m_uncaughtExceptionHandler = NULL;
  m_threadHandle             = CreateThread(NULL, stackSize,(LPTHREAD_START_ROUTINE)threadStartup, this, CREATE_SUSPENDED, &m_threadId);
  if(m_threadHandle == NULL) {
    throwMethodLastErrorOnSysCallException(s_className, _T("CreateThread"));
  }
  HRESULT hr = SetThreadDescription(m_threadHandle, description.cstr());
  CHECKRESULT(hr);

  getMap().addThread(this);
}

Thread::~Thread() {
  if(!m_isDemon) {
    m_terminated.wait();
  }
  getMap().removeThread(this);
  CloseHandle(m_threadHandle);
}

void Thread::handleUncaughtException(Exception &e) {
  if(m_uncaughtExceptionHandler != NULL) {
    m_uncaughtExceptionHandler->uncaughtException(*this, e);
  } else {
    s_defaultUncaughtExceptionHandler->uncaughtException(*this, e);
  }
}

void Thread::setDesription(const String &description) {
  ::setThreadDescription(m_threadHandle, description);
}

String Thread::getDescription() const {
  return ::getThreadDescription(m_threadHandle);
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

void Thread::addPropertyChangeListener(PropertyChangeListener *listener) { // static 
  getMap().addListener(listener);
}

void Thread::removePropertyChangeListener(PropertyChangeListener *listener) { // static 
  getMap().removeListener(listener);
}

ULONG Thread::getExitCode() const {
  DWORD exitCode = 0;
  if(!GetExitCodeThread(m_threadHandle,&exitCode)) {
    throwLastErrorOnSysCallException(__TFUNCTION__);
  }
  return exitCode;
}

bool Thread::stillActive() const {
  return getExitCode() == STILL_ACTIVE;
}

void Thread::setPriority(int priority) {
  if(!SetThreadPriority(m_threadHandle,priority)) {
    throwLastErrorOnSysCallException(__TFUNCTION__);
  }
}

int Thread::getPriority() const {
  return GetThreadPriority(m_threadHandle);
}

void Thread::setPriorityBoost(bool disablePriorityBoost) {
  if(!SetThreadPriorityBoost(m_threadHandle, disablePriorityBoost)) {
    throwLastErrorOnSysCallException(__TFUNCTION__);
  }
}

bool Thread::getPriorityBoost() const {
  BOOL boostDisabled;
  if(GetThreadPriorityBoost(m_threadHandle, &boostDisabled)) {
    throwLastErrorOnSysCallException(__TFUNCTION__);
  }
  return boostDisabled ? true : false;
}

void Thread::setAffinityMask(DWORD mask) {
  if(!SetThreadAffinityMask(m_threadHandle,mask)) {
    throwLastErrorOnSysCallException(__TFUNCTION__);
  }
}

void Thread::setIdealProcessor(DWORD cpu) {
  if(SetThreadIdealProcessor(m_threadHandle,cpu) == (DWORD)-1) {
    throwLastErrorOnSysCallException(__TFUNCTION__);
  }
}

double Thread::getThreadTime() {
  return ::getThreadTime(m_threadHandle);
}

Thread *Thread::getThreadById(DWORD threadId) { // static
  return getMap().findThread(threadId);
}

EXECUTION_STATE Thread::setExecutionState(EXECUTION_STATE newState) { // static
  return SetThreadExecutionState(newState);
}
