#include "pch.h"

BigRealThread::BigRealThread(int id)
: Thread(format(_T("NT%03d"), id))
, BigRealResource(id)
, m_resultQueue(NULL)
, m_execute(0)
{
  setDeamon(true);
  m_requestCount = 0;
  start();
}

unsigned int BigRealThread::run() {
  FPU::setPrecisionMode(FPU_HIGH_PRECISION);
  for(;;) {
    try {
      m_execute.wait();
      m_requestCount++;
      m_job->run();
      m_resultQueue->put(NULL);
    } catch(Exception e) {
      m_resultQueue->put(STRDUP(e.what())); // indicating an error
    } catch(...) {
      m_resultQueue->put(STRDUP(format(_T("Unknown exception received in BigRealThread %s"), getName().cstr()).cstr()));
    }
  }
  return 0;
}

void BigRealThread::execute(Runnable &job, SynchronizedStringQueue &resultQueue) {
  m_job         = &job;
  m_resultQueue = &resultQueue;
  m_execute.signal();
}
