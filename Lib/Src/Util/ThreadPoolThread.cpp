#include "pch.h"
#include "ThreadPoolInternal.h"

#define THR_BUSY             0x01
#define THR_REQUESTTERMINATE 0x02
#define THR_TERMINATED       0x04

#ifdef TRACE_THREADPOOL
static String flagsToString(BYTE flags) {
  String result;
#define addFlag(f) if(flags & THR_##f) { result += _T(" "); result += _T(#f); }
  addFlag(BUSY)
    addFlag(REQUESTTERMINATE)
    addFlag(TERMINATED)
    return result;
#undef addFlag
}
#define FLGSTR() flagsToString(m_flags).cstr()
#endif


IdentifiedThread::IdentifiedThread(IdentifiedThreadPool *pool, int id)
: Thread(format(_T("TPT%03d"), id))
, IdentifiedResource(id)
, m_pool(           *pool)
, m_resultQueue(     NULL)
, m_flags(           0)
, m_execute(         0)
{
  TRACE(_T("%s(id=%d) called\n"), __TFUNCTION__,id);
  setDemon(false);
  m_requestCount = 0;
  start();
  TRACE(_T("%s(%d) done\n"), __TFUNCTION__,id);
}

IdentifiedThread::~IdentifiedThread() {
  TRACE(_T("%s(id=%d):state:%s\n"), __TFUNCTION__,IdentifiedResource::getId(), FLGSTR());
}

void IdentifiedThread::requestTerminate() {
  if((m_flags & THR_REQUESTTERMINATE) == 0) {
    m_flags |= THR_REQUESTTERMINATE;
    if((m_flags & THR_BUSY) == 0) m_execute.notify();
  }
}

UINT IdentifiedThread::run() {
//#ifdef _DEBUG
  m_pool.incrActiveCount();
  const String oldDesc = getDescription();
//#endif
  for(;;) {
    try {
      if(m_flags & THR_REQUESTTERMINATE) break;
      TRACE(_T("%s(%s) waiting for job (state:%s)\n"), __TFUNCTION__, oldDesc.cstr(), FLGSTR());
      m_execute.wait();
      if(m_flags & THR_REQUESTTERMINATE) break;
      m_requestCount++;
      m_flags |= THR_BUSY;
      TRACE(_T("%s(%s) now running job %d (state=%s)\n"), __TFUNCTION__, oldDesc.cstr(), m_requestCount, FLGSTR());
      m_job->run();
      if(m_resultQueue) m_resultQueue->putAllDone();
    } catch(Exception e) {
      if(m_resultQueue) m_resultQueue->putError(e.what()); // indicating an error
    } catch(...) {
      if(m_resultQueue) m_resultQueue->putError(format(_T("Unknown exception received in IdentifiedThread %s"), getDescription().cstr()));
    }
    m_resultQueue = NULL;
    m_flags &= ~THR_BUSY;
#ifdef _DEBUG
    setDescription(oldDesc);
#endif
    TRACE(_T("%s(%s) finished job %d (state=%s)\n"), __TFUNCTION__, oldDesc.cstr(), m_requestCount, FLGSTR());
    ThreadPool::releaseThread(this);
  }
  m_flags |= THR_TERMINATED;
  TRACE(_T("Thread(%s) terminated (state=%s)\n"), oldDesc.cstr(), FLGSTR());
  m_pool.decrActiveCount();
  return 0;
}

void IdentifiedThread::executeJob(Runnable &job, IdentifiedResultQueue *resultQueue) {
  m_job         = &job;
  m_resultQueue = resultQueue;
  m_execute.notify();
}

void IdentifiedResultQueue::waitForResults(size_t expectedResultCount) {
  String errorMsg;
  bool gotException = false;
  for(;expectedResultCount > 0; expectedResultCount--) {
    String *str = get();
    if(str != NULL) {
      if(!gotException || (errorMsg.length() == 0)) {
        errorMsg = *str;
      }
      gotException = true;
      delete str;
    }
  }
  if(gotException) {
    throwException(errorMsg);
  }
}
