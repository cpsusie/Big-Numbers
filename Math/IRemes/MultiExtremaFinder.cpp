#include "StdAfx.h"
#include <ThreadPool.h>
#include <Thread.h>
#include <CPUInfo.h>
#include "MultiExtremaFinder.h"

#ifdef MULTITHREADEDEXTREMAFINDER

class ExtremumSearchParam {
  friend class ExtremumFinder;
public:
  const int m_index;
  BigReal   m_left, m_right, m_middle;
  ExtremumSearchParam::ExtremumSearchParam(int index, const BigReal &l, const BigReal &r, const BigReal &m)
    : m_index(index)
    , m_left(l)
    , m_right(r)
    , m_middle(m)
  {
  }
};

class ExtremumResult {
public:
  const int    m_index;
  ConstBigReal m_extr;
  ExtremumResult(int index, const BigReal &extr) : m_index(index), m_extr(extr) {
  }
};

class ExtremumFinder : public Runnable {
private:
  MultiExtremaFinder &m_mf;
  const int           m_id;
  DigitPool          *m_pool;
  RationalFunction   &m_approx;
public:
  ExtremumFinder(MultiExtremaFinder *mf, int id) : m_mf(*mf), m_id(id), m_pool(NULL), m_approx(mf->m_approx) {
    m_pool = BigRealResourcePool::fetchDigitPool();
  }
  ~ExtremumFinder() {
    if(m_pool) {
      BigRealResourcePool::releaseDigitPool(m_pool);
    }
  }
  UINT run();
};

UINT ExtremumFinder::run() {
#ifdef _DEBUG
  setThreadDescription(format(_T("ExtrFinder(%d),pool=%d"), m_id, m_pool->getId()));
#endif
  ExtremumSearchParamQueue &paramQueue = m_mf.m_paramQueue;
  Remes                    &remes      = m_mf.m_remes;
  try {
    BigReal                   result(m_pool);
    for(;;) {
      try {
        const ExtremumSearchParam *param = paramQueue.get(10);
        result = remes.findExtremum(m_approx, param->m_left, param->m_middle, param->m_right, m_pool);
        m_mf.putExtremum(param->m_index, result);
      } catch(TimeoutException e) {
        if(paramQueue.isEmpty()) {
          break;
        }
      }
    }
  } catch(...) {
    m_mf.putTerminationCode();
  }
  return 0;
}

void MultiExtremaFinder::insertJob(int index, const BigReal &l, const BigReal &r, const BigReal &m) {
  ExtremumSearchParam *p = new ExtremumSearchParam(index, l, r, m); TRACE_NEW(p);
  m_paramQueue.put(p);
  m_paramArray.add(p);
}

void MultiExtremaFinder::putExtremum(int index, const BigReal &extremum) {
  const ExtremumResult *r = new ExtremumResult(index, extremum); TRACE_NEW(r);
  m_resultQueue.put(r);
}

void MultiExtremaFinder::putTerminationCode() {
  m_resultQueue.put(NULL);
}

class ExtremumNotifier : public Runnable {
private:
  MultiExtremaFinder &m_mf;
  const size_t        m_expectedResultCount;
public:
  ExtremumNotifier(MultiExtremaFinder *mf, size_t expectedResultCount)
    : m_mf(*mf)
    , m_expectedResultCount(expectedResultCount)
  {
  }
  UINT run();
};

UINT ExtremumNotifier::run() {
#ifdef _DEBUG
  setThreadDescription("NOTIFIER");
#endif
  Remes       &remes = m_mf.m_remes;
  ResultQueue &q     = m_mf.m_resultQueue;

  for(size_t i = 0; i < m_expectedResultCount; i++) {
    const ExtremumResult *r = q.get();
    if(r == NULL) break; // terminationCode
    remes.setExtremum(r->m_index, r->m_extr);
    m_mf.m_resultArray.add(r);
  }
  return 0;
}

MultiExtremaFinder::MultiExtremaFinder(Remes *remes) : m_remes(*remes) {
  m_remes.getCurrentApproximation(m_approx);
}

MultiExtremaFinder::~MultiExtremaFinder() {
  m_paramQueue.clear();
  m_resultQueue.clear();
  for(size_t i = 0; i < m_jobArray.size(); i++) {
    Runnable *r = m_jobArray[i];
    SAFEDELETE(r);
  }
  m_jobArray.clear();

  for(size_t i = 0; i < m_paramArray.size(); i++) {
    const ExtremumSearchParam *p = m_paramArray[i];
    SAFEDELETE(p);
  }
  m_paramArray.clear();

  for(size_t i = 0; i < m_resultArray.size(); i++) {
    const ExtremumResult *r = m_resultArray[i];
    SAFEDELETE(r);
  }
  m_resultArray.clear();
}

void MultiExtremaFinder::findAllExtrema() {
  m_resultArray.setCapacity(m_paramArray.getCapacity());
  const int pCount = getProcessorCount();
  m_jobArray.setCapacity(pCount+1);
  for(int i = 0; i < pCount; i++) {
    ExtremumFinder *f = new ExtremumFinder(this,i); TRACE_NEW(e);
    m_jobArray.add(f);
  }
  ExtremumNotifier *notifier = new ExtremumNotifier(this, m_paramQueue.size()); TRACE_NEW(notifier);
  m_jobArray.add(notifier);

  ThreadPool::executeInParallel(m_jobArray);
}

#endif // MULTITHREADEDEXTREMAFINDER
