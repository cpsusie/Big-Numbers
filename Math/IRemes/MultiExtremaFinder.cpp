#include "StdAfx.h"
#include "MultiExtremaFinder.h"

#ifdef MULTITHREADEDEXTREMAFINDER

ExtremaSearchJob::ExtremaSearchJob(Remes &remes, int index, const BigReal &l, const BigReal &r, const BigReal &m, DigitPool *pool)
  : m_remes(remes)
  , m_index(index)
  , m_left(pool)
  , m_right(pool)
  , m_middle(pool)
  , m_pool(pool)
  , m_result(pool)
{
  m_left   = l;
  m_right  = r;
  m_middle = m;
}

ExtremaSearchJob::~ExtremaSearchJob() {
  m_left = m_right = m_middle = 0;
  BigRealResourcePool::releaseDigitPool(m_pool);
}

unsigned int ExtremaSearchJob::run() {
  m_result = m_remes.findExtremum(m_left, m_middle, m_right, m_pool);
  return 0;
}

unsigned int WorkerThread::run() {
  for (;;) {
    try {
      ExtremaSearchJob *job = m_jobQueue.get(10);
      job->run();
      m_resultQueue.put(ExtremumResult(job->getResult(), job->getIndex()));
    } catch (TimeoutException e) {
      if(m_jobQueue.isEmpty()) {
        return 0;
      }
    }
  }
}

MultiExtremaFinder::~MultiExtremaFinder() {
  m_jobQueue.clear();
  for(size_t i = 0; i < m_allJobs.size(); i++) {
    delete m_allJobs[i];
  }
  m_allJobs.clear();
  for(size_t i = 0; i < m_threads.size(); i++) {
    delete m_threads[i];
  }
  m_threads.clear();
}

void MultiExtremaFinder::insertJob(int index, const BigReal &l, const BigReal &r, const BigReal &m) {
  m_allJobs.add(new ExtremaSearchJob(m_remes, index, l, r, m, BigRealResourcePool::fetchDigitPool()));
}

void MultiExtremaFinder::waitUntilAllThreadsTerminated() {
  int runningCount;
  for(;;) {
    runningCount = 0;
    for(size_t i = 0; i < m_threads.size(); i++) {
      if(m_threads[i]->stillActive()) {
        runningCount++;
      }
    }
    if(runningCount > 0) {
      Sleep(20);
    } else {
      break;
    }
  }
}

void MultiExtremaFinder::execute() {
  for(size_t i = 0; i < m_allJobs.size(); i++) {
    m_jobQueue.put(m_allJobs[i]);
  }
  ExtremaResultQueue resultQueue;
  for(int i = 0; i < m_processorCount; i++) {
    WorkerThread *thr = new WorkerThread(m_jobQueue, resultQueue);
    m_threads.add(thr);
    thr->start();
  }
  try {
    for(size_t count = 0; count < m_allJobs.size(); count++) {
      ExtremumResult result(&DEFAULT_DIGITPOOL);
      result = resultQueue.get();
      m_remes.setExtremum(result.m_index, result);
    }
  } catch (...) { // terminate All threads
    m_jobQueue.clear();
    waitUntilAllThreadsTerminated();
    throw;
  }
}

#endif // MULTITHREADEDEXTREMAFINDER
