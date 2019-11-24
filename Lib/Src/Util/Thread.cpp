#include "pch.h"
#include <Process.h>
#include <processthreadsapi.h>
#include <io.h>
#include <MyUtil.h>
#include <SingletonFactory.h>
#include <Thread.h>
#include <FastSemaphore.h>
#include <HashSet.h>
#include <eh.h>

DEFINECLASSNAME(Thread);

class DefaultExceptionHandler : public UncaughtExceptionHandler {
public:
  void uncaughtException(Thread &thread, Exception &e);
};

void DefaultExceptionHandler::uncaughtException(Thread &thread, Exception &e) {
  String errorText = format(_T("Uncaught Exception in thread %s(%lu)\n%s\n"), thread.getName().cstr(), thread.getId(), e.what());
  if(isatty(stderr)) {
    _ftprintf(stderr, _T("\n%s\n"), errorText.cstr());
  } else {
    MessageBox(NULL, errorText.replace(_T('\n'), _T("\r\n")).cstr(), _T("Error"), MB_OK | MB_ICONERROR);
  }
  abort();
}

static ULONG threadHash(const Thread * const &t) {
  return t->getId();
}

static int threadCmp(const Thread * const &t1, const Thread * const &t2) {
  return (long)t1->getId() - (long)t2->getId();
}

class ThreadSet : private HashSet<const Thread*> {
private:
  mutable FastSemaphore m_lock, m_isEmpty;

  void killDeamonThreads();
  void waitUntilEmpty();
public:
  ThreadSet() : HashSet<const Thread*>(threadHash, threadCmp) {
    Thread::s_defaultUncaughtExceptionHandler = new DefaultExceptionHandler;
  }

  ~ThreadSet() { // declared virtual in Collection
    killDeamonThreads();
    waitUntilEmpty();
    SAFEDELETE(Thread::s_defaultUncaughtExceptionHandler);
  }
  void addThread(const Thread *thread) {
    m_lock.wait();
    if(__super::isEmpty()) m_isEmpty.wait();
    add(thread);
    m_lock.notify();
  }
  void removeThread(const Thread *thread) {
    m_lock.wait();
    remove(thread);
    if(__super::isEmpty()) m_isEmpty.notify();
    m_lock.notify();
  }
  inline bool isEmpty() const {
    m_lock.wait();
    const bool result = __super::isEmpty();
    m_lock.notify();
    return result;
  }
  static ThreadSet &getInstance();
};

ThreadSet &ThreadSet::getInstance() {
  static SingletonFactoryTemplate<ThreadSet> factory;
  return factory.getInstance();
}

void ThreadSet::killDeamonThreads() {
  m_lock.wait();
  for(Iterator<const Thread*> it = getIterator(); it.hasNext();) {
    const Thread *thread = it.next();
    if(thread->isDeamon()) {
//      TerminateThread(thread->m_threadHandle, 0);
      it.remove();
    }
  }
  m_lock.notify();
}

void ThreadSet::waitUntilEmpty() {
  while(!isEmpty()) {
    m_isEmpty.wait();
  }
}

static UINT threadStartup(Thread *thread) {
  thread->m_terminated.wait();
  _set_se_translator(exceptionTranslator);
  ThreadSet &thrSet = ThreadSet::getInstance();
  thrSet.addThread(thread);
  UINT result = -1;
  try {
    result = thread->run();
    thrSet.removeThread(thread);
    thread->m_terminated.notify();
  } catch(Exception e) {
    thrSet.removeThread(thread);
    thread->m_terminated.notify();
    thread->handleUncaughtException(e);
  } catch(...) {
    thrSet.removeThread(thread);
    thread->m_terminated.notify();
    thread->handleUncaughtException(Exception(_T("Unknown Exception")));
  }
  return result;
}

UncaughtExceptionHandler *Thread::s_defaultUncaughtExceptionHandler = NULL;

Thread::Thread(Runnable &target, const String &name, size_t stackSize) {
  init(&target, name, stackSize);
}

Thread::Thread(const String &name, size_t stackSize) {
  init(NULL, name, stackSize);
}

void Thread::init(Runnable *target, const String &name, size_t stackSize) {
  m_target                   = target;
  m_name                     = name;
  m_isDeamon                 = false;
  m_uncaughtExceptionHandler = NULL;
  m_threadHandle             = CreateThread(NULL, stackSize,(LPTHREAD_START_ROUTINE)threadStartup, this, CREATE_SUSPENDED, &m_threadId);
  if(m_threadHandle == NULL) {
    throwMethodLastErrorOnSysCallException(s_className, _T("CreateThread"));
  }
}

Thread::~Thread() {
  if(!m_isDeamon) {
    m_terminated.wait();
  } else {
    ThreadSet::getInstance().removeThread(this);
  }
  CloseHandle(m_threadHandle);
}

void Thread::handleUncaughtException(Exception &e) {
  if(m_uncaughtExceptionHandler != NULL) {
    m_uncaughtExceptionHandler->uncaughtException(*this, e);
  } else {
    s_defaultUncaughtExceptionHandler->uncaughtException(*this, e);
  }
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

EXECUTION_STATE Thread::setExecutionState(EXECUTION_STATE newState) { // static
  return SetThreadExecutionState(newState);
}
