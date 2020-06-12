#include "pch.h"
#include "ThreadPoolInternal.h"

#define THR_BUSY             0x01
#define THR_REQUESTTERMINATE 0x02
#define THR_TERMINATED       0x04

#if defined(TRACE_THREADPOOL)
static String flagsToString(BYTE flags) {
  if(flags == 0) {
    return _T("IDLE (0)");
  }
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


ThreadPoolThread::ThreadPoolThread(PoolThreadPool *pool, UINT id, const String &name)
: Thread(name)
, IdentifiedResource(id)
, m_pool(           *pool)
, m_resultQueue(     NULL)
, m_flags(           0)
, m_execute(         0)
{
  THREADPOOL_TRACE("%s(id=%d) called\n", __TFUNCTION__,id);
  setDemon(false);
  m_requestCount = 0;
  start();
  THREADPOOL_TRACE("%s(%d) done\n", __TFUNCTION__,id);
}

ThreadPoolThread::~ThreadPoolThread() {
  THREADPOOL_TRACE("%s(id=%d):state:%s\n", __TFUNCTION__,getResourceId(), FLGSTR());
}

void ThreadPoolThread::requestTerminate() {
  if(!isSet(THR_REQUESTTERMINATE)) {
    setFlag(THR_REQUESTTERMINATE);
    if(!isSet(THR_BUSY)) {
      m_execute.notify();
    }
  }
}

UINT ThreadPoolThread::run() {
//#if defined(_DEBUG)
  m_pool.incrActiveCount();
  const String oldDesc = getDescription();
//#endif
  for(;;) {
    try {
      if(isSet(THR_REQUESTTERMINATE)) {
        break;
      }
      THREADPOOL_TRACE("%s(%s) waiting for job (state:%s)\n", __TFUNCTION__, oldDesc.cstr(), FLGSTR());
      m_execute.wait();
      if(isSet(THR_REQUESTTERMINATE)) {
        break;
      }
      m_requestCount++;
      setFlag(THR_BUSY);
      THREADPOOL_TRACE("%s(%s) now running job %d (state=%s)\n", __TFUNCTION__, oldDesc.cstr(), m_requestCount, FLGSTR());
      m_job->run();
      if(m_resultQueue) m_resultQueue->putAllDone();
    } catch(Exception e) {
      if(m_resultQueue) m_resultQueue->putError(e.what()); // indicating an error
    } catch(...) {
      if(m_resultQueue) m_resultQueue->putError(format(_T("Unknown exception received in ThreadPoolThread %s"), getDescription().cstr()));
    }
    m_resultQueue = NULL;
    clrFlag(THR_BUSY);
#if defined(_DEBUG)
    setDescription(oldDesc);
#endif
    THREADPOOL_TRACE("%s(%s) finished job %d (state=%s)\n", __TFUNCTION__, oldDesc.cstr(), m_requestCount, FLGSTR());
    m_pool.getThreadPool().releaseThread(this);
  }
  setFlag(THR_TERMINATED);
  THREADPOOL_TRACE("Thread(%s) terminated (state=%s)\n", oldDesc.cstr(), FLGSTR());
  m_pool.decrActiveCount();
  return 0;
}

void ThreadPoolThread::executeJob(Runnable &job, ThreadPoolResultQueue *resultQueue) {
  m_job         = &job;
  m_resultQueue = resultQueue;
  m_execute.notify();
}

void ThreadPoolResultQueue::waitForResults(size_t expectedResultCount) {
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
