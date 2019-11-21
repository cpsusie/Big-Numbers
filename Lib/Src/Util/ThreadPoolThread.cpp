#include "pch.h"
#include <MyUtil.h>
#include <Thread.h>

ThreadPoolThread::ThreadPoolThread(int id)
: Thread(format(_T("TPT%03d"), id))
, IdentifiedResource(id)
, m_resultQueue(NULL)
, m_execute(0)
{
  setDeamon(true);
  m_requestCount = 0;
  start();
}

UINT ThreadPoolThread::run() {
  for(;;) {
    try {
      m_execute.wait();
      m_requestCount++;
      m_job->run();
      if(m_resultQueue) m_resultQueue->put(NULL);
    } catch(Exception e) {
      if(m_resultQueue) m_resultQueue->put(STRDUP(e.what())); // indicating an error
    } catch(...) {
      if(m_resultQueue) m_resultQueue->put(STRDUP(format(_T("Unknown exception received in BigRealThread %s"), getName().cstr()).cstr()));
    }
    m_resultQueue = NULL;
    ThreadPool::releaseThread(this);
  }
  return 0;
}

void ThreadPoolThread::execute(Runnable &job, ThreadPoolResultQueue *resultQueue) {
  m_job         = &job;
  m_resultQueue = resultQueue;
  m_execute.notify();
}

void ThreadPoolResultQueue::waitForResults(size_t expectedResultCount) {
  String errorMsg;
  bool gotException = false;
  for(;expectedResultCount > 0; expectedResultCount--) {
    TCHAR *str = get();
    if(str != NULL) {
      if(!gotException || (errorMsg.length() == 0)) {
        errorMsg = str;
      }
      gotException = true;
      FREE(str);
    }
  }
  if(gotException) {
    throwException(errorMsg);
  }
}
