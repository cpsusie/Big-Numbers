#include "pch.h"
#include "ThreadPoolInternal.h"

IdentifiedThread::IdentifiedThread(int id)
: Thread(format(_T("TPT%03d"), id))
, IdentifiedResource(id)
, m_resultQueue(NULL)
, m_execute(0)
{
  setDeamon(true);
  m_requestCount = 0;
  start();
}

UINT IdentifiedThread::run() {
  for(;;) {
    try {
      m_execute.wait();
      m_requestCount++;
      m_job->run();
      if(m_resultQueue) m_resultQueue->putAllDone();
    } catch(Exception e) {
      if(m_resultQueue) m_resultQueue->putError(e.what()); // indicating an error
    } catch(...) {
      if(m_resultQueue) m_resultQueue->putError(format(_T("Unknown exception received in IdentifiedThread %s"), getName().cstr()));
    }
    m_resultQueue = NULL;
    ThreadPool::releaseThread(this);
  }
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
