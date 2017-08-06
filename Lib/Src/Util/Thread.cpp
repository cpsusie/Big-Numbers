#include "pch.h"
#include <Process.h>
#include <processthreadsapi.h>
#include <io.h>
#include <Thread.h>
#include <HashSet.h>
#include <eh.h>

DEFINECLASSNAME(Thread);

static ULONG threadHash(const Thread * const &t) {
  return t->getId();
}

static int threadCmp(const Thread * const &t1, const Thread * const &t2) {
  return (long)t1->getId() - (long)t2->getId();
}

class ThreadSet : private HashSet<const Thread*> {
private:
  Semaphore m_gate;
public:
  ThreadSet() : HashSet<const Thread*>(threadHash, threadCmp) {
  }

  void addThread(const Thread *thread) {
    m_gate.wait();
    add(thread);
    m_gate.signal();
  }
  void removeThread(const Thread *thread) {
    m_gate.wait();
    remove(thread);
    m_gate.signal();
  }
  void killDeamonThreads();
  inline bool isEmpty() const {
    return __super::isEmpty();
  }
};

void ThreadSet::killDeamonThreads() {
  m_gate.wait();
  for(Iterator<const Thread*> it = getIterator(); it.hasNext();) {
    const Thread *thread = it.next();
    if(thread->isDeamon()) {
//      TerminateThread(thread->m_threadHandle, 0);
      it.remove();
    }
  }
  m_gate.signal();
}

static ThreadSet *runningThreadSet = NULL;

static UINT threadStartup(Thread *thread) {
  thread->m_gate.wait();
  _set_se_translator(exceptionTranslator);
  runningThreadSet->addThread(thread);
  UINT result = -1;
  try {
    result = thread->run();
    runningThreadSet->removeThread(thread);
    thread->m_gate.signal();
  } catch(Exception e) {
    runningThreadSet->removeThread(thread);
    thread->m_gate.signal();
    thread->handleUncaughtException(e);
  } catch(...) {
    runningThreadSet->removeThread(thread);
    thread->m_gate.signal();
    thread->handleUncaughtException(Exception(_T("Unknown Exception")));
  }
  return result;
}


class DefaultExceptionHandler : public UncaughtExceptionHandler {
public:
  void uncaughtException(Thread &thread, Exception &e);
};

void DefaultExceptionHandler::uncaughtException(Thread &thread, Exception &e) {
  String errorText = format(_T("Uncaught Exception in thread %s(%lu)\n%s\n"), thread.getName().cstr(), thread.getId(), e.what());
  if(isatty(stderr)) {
    _ftprintf(stderr, _T("\n%s\n"), errorText.cstr());
  } else {
    MessageBox(NULL, errorText. replace(_T('\n'),_T("\r\n")).cstr(), _T("Error"), MB_OK | MB_ICONERROR);
  }
  abort();
}

static DefaultExceptionHandler _defaultExceptionHandler;

UncaughtExceptionHandler *Thread::s_defaultUncaughtExceptionHandler;

#pragma warning(disable : 4073)
#pragma init_seg(lib)

class InitThreadClass {
public:
  InitThreadClass();
 ~InitThreadClass();
};

InitThreadClass::InitThreadClass() {
  runningThreadSet = new ThreadSet; TRACE_NEW(runningThreadSet);
  Thread::s_defaultUncaughtExceptionHandler = &_defaultExceptionHandler;
}

InitThreadClass::~InitThreadClass() {
  runningThreadSet->killDeamonThreads();
  if(runningThreadSet->isEmpty()) {
    SAFEDELETE(runningThreadSet);
  }
}

static InitThreadClass ThreadClassInitializer;

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
    m_gate.wait();
  } else {
    runningThreadSet->removeThread(this);
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
