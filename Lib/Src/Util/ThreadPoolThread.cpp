#include "pch.h"
#include "ThreadPoolInternal.h"

IdentifiedThread::IdentifiedThread(IdentifiedThreadPool *pool, int id)
: Thread(format(_T("TPT%03d"), id))
, IdentifiedResource(id)
, m_pool(           *pool)
, m_resultQueue(     NULL)
, m_requestTerminate(false)
, m_busy(            false)
, m_execute(0)
{
  setDemon(false);
  m_requestCount = 0;
  start();
}

IdentifiedThread::~IdentifiedThread() {
  requestTerminate();
}

void IdentifiedThread::requestTerminate() {
  if(!m_requestTerminate) {
    m_requestTerminate = true;
    if(!m_busy) m_execute.notify();
  }
}

UINT IdentifiedThread::run() {
  const String oldDesc = getDescription();
  for(;;) {
    try {
      if(m_requestTerminate) break;
      m_execute.wait();
      if(m_requestTerminate) break;
      m_requestCount++;
      m_busy = true;
      m_job->run();
      if(m_resultQueue) m_resultQueue->putAllDone();
    } catch(Exception e) {
      if(m_resultQueue) m_resultQueue->putError(e.what()); // indicating an error
    } catch(...) {
      if(m_resultQueue) m_resultQueue->putError(format(_T("Unknown exception received in IdentifiedThread %s"), getDescription().cstr()));
    }
    m_resultQueue = NULL;
    m_busy        = false;
    setDesription(oldDesc);
    ThreadPool::releaseThread(this);
  }
  m_pool.incrTerminatedThreads();
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
