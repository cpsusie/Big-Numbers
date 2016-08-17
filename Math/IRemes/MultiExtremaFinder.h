#pragma once

#ifdef MULTITHREADEDEXTREMAFINDER

#include <CPUInfo.h>

class ExtremaSearchJob : public Runnable {
private:
  Remes       &m_remes;
  DigitPool   *m_pool;
  BigReal      m_left, m_right, m_middle, m_result;
  int          m_index;
public:
  unsigned int run();
  ExtremaSearchJob(Remes &remes, int index, const BigReal &l, const BigReal &r, const BigReal &m, DigitPool *pool);
  ~ExtremaSearchJob();
  const BigReal &getResult() const {
    return m_result;
  }
  int getIndex() const {
    return m_index;
  }
};

class ExtremumResult : public BigReal {
public:
  int m_index;
  ExtremumResult(DigitPool *pool) : BigReal(pool), m_index(-1) {
  }
  ExtremumResult(const BigReal &extr, int index) : BigReal(extr), m_index(index) {
  }
};

typedef SynchronizedQueue<ExtremaSearchJob*> ExtremaJobQueue;
typedef SynchronizedQueue<ExtremumResult>    ExtremaResultQueue;

class WorkerThread : public Thread {
private:
  ExtremaJobQueue    &m_jobQueue;
  ExtremaResultQueue &m_resultQueue;
public:
  WorkerThread(ExtremaJobQueue &jobQueue, ExtremaResultQueue &resultQueue) 
    : m_jobQueue(jobQueue)
    , m_resultQueue(resultQueue)
  {
  }
  unsigned int run();
};

class MultiExtremaFinder {
private:
  const int                       m_processorCount;
  Remes                          &m_remes;
  CompactArray<WorkerThread*>     m_threads;
  ExtremaJobQueue                 m_jobQueue;
  CompactArray<ExtremaSearchJob*> m_allJobs;
  void waitUntilAllThreadsTerminated();

public:
  MultiExtremaFinder(Remes *remes) : m_remes(*remes), m_processorCount(getProcessorCount()) {
  }
  ~MultiExtremaFinder();
  void insertJob(int index, const BigReal &l, const BigReal &r, const BigReal &m);
  void execute();
};

#endif
