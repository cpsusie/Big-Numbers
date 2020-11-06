#include "pch.h"
#include <Thread.h>
#include <ThreadPool.h>
#include <BitSet.h>
#include <CPUInfo.h>
#include <SynchronizedQueue.h>
#include <Math/BigReal/BigRealResourcePool.h>
#include <Math/BigReal/MRisprime.h>

static BigInt randomOddInteger(int digits, RandomGenerator &rnd, DigitPool *pool) {
  BigInt n = randBigInt(digits, rnd, pool);
  if(isEven(n)) ++n;
  return n;
}

#define DEFAULT_SIEVESIZE 30000
class SmallPrimeSet {
private:
  BitSet m_primeSet;
public:
  SmallPrimeSet(size_t sieveSize = DEFAULT_SIEVESIZE);
  // Return max value to check
  inline size_t getMax() const {
    return m_primeSet.getCapacity();
  }
  Iterator<size_t> getIterator() const {
    return ((BitSet&)m_primeSet).getIterator();
  }
};

SmallPrimeSet::SmallPrimeSet(size_t sieveSize) : m_primeSet(sieveSize) {
  m_primeSet.clear();
  m_primeSet.invert();
  m_primeSet.remove(0,1);
  for(size_t i = 2; i < sieveSize; i++) {
    if(m_primeSet.contains(i)) {
      for(int j = i+i; j < sieveSize; j+=i) {
        m_primeSet.remove(j);
      }
    }
  }
}

class Sieve {
private:
  static const SmallPrimeSet s_smallPrimeSet;
  const BigInt               m_base;
  UINT                      *m_remainder;
public:
  Sieve(const BigInt &base);
  ~Sieve() {
    SAFEDELETEARRAY(m_remainder);
  }
  bool hasSmallFactor(const BigInt &n, int &factor) const;
};

const SmallPrimeSet Sieve::s_smallPrimeSet;

Sieve::Sieve(const BigInt &base) : m_base(base) {
  DigitPool   *pool = base.getDigitPool();
  const size_t n    = s_smallPrimeSet.getMax();
  m_remainder = new UINT[n]; TRACE_NEW(m_remainder);
  memset(m_remainder,0,sizeof(m_remainder[0])*n);
  BigInt bp(pool), r(pool);
  for(Iterator<size_t> it = s_smallPrimeSet.getIterator(); it.hasNext(); ) {
    const UINT p = it.next();
    bp = p;
    m_remainder[p] = (UINT)(m_base % bp);
  }
}

bool Sieve::hasSmallFactor(const BigInt &n, int &factor) const {
  const int diff = (int)(n - m_base);
  for(Iterator<size_t> it = s_smallPrimeSet.getIterator(); it.hasNext(); ) {
    int p = (int)it.next();
    int r;
    if(diff > 0) {
      r = diff - (p-m_remainder[p]);
    } else {
      r = diff - m_remainder[p];
    }
    if(r == 0) continue;
    if(r % p == 0) {
      factor = p;
      return true;
    }
  }
  return false;
}

class PrimeMonitor;

class PrimeSearcher : public Runnable {
private:
  PrimeMonitor  &m_mon;
  const int      m_id;
  DigitPool     *m_pool;
  Semaphore      m_terminated;
public:
  PrimeSearcher(PrimeMonitor *mon, int id);
  ~PrimeSearcher();
  UINT run();
  inline void stopSearch() {
    m_pool->terminatePoolCalculation();
  }
};

class PrimeMonitor {
  friend class PrimeSearcher;
private:
  const int                    m_digitCount;
  DigitPool                   *m_digitPool;
  MillerRabinHandler          *m_handler;
  CompactArray<PrimeSearcher*> m_jobs;
  std::atomic<BYTE>            m_runningCount;
  mutable Semaphore            m_lock;
  Array<BigInt>                m_result;
public:
  PrimeMonitor(int digitCount, int threadCount, DigitPool *pool, MillerRabinHandler *handler);
  ~PrimeMonitor();
  void startJobs();
  void terminateJobs();
  inline int getPrimesFound() const {
    return m_result.size();
  }
  void putPrime(const BigInt &p);
  Array<BigInt> getResult() const;
};

PrimeMonitor::PrimeMonitor(int digitCount, int threadCount, DigitPool *digitPool, MillerRabinHandler *handler)
: m_digitCount(digitCount)
, m_digitPool( digitPool )
, m_handler(   handler   )
, m_runningCount(0       )
{
  threadCount = max(1, threadCount);
  const int processorCount = getProcessorCount();
  threadCount = min(threadCount,processorCount);
  for(int i = 0; i < threadCount; i++) {
    PrimeSearcher *ps = new PrimeSearcher(this, i); TRACE_NEW(ps);
    m_jobs.add(ps);
  }
}

PrimeMonitor::~PrimeMonitor() {
  terminateJobs();
  for(size_t i = 0; i < m_jobs.size(); i++) {
    SAFEDELETE(m_jobs[i]);
  }
  m_jobs.clear();
}

void PrimeMonitor::putPrime(const BigInt &p) {
  m_lock.wait();
  m_result.add(BigInt(p, m_digitPool));
  m_lock.notify();
}

void PrimeMonitor::startJobs() {
  RunnableArray ra(m_jobs.size());
  for(size_t i = 0; i < m_jobs.size(); i++) {
    ra.add(m_jobs[i]);
  }
  ThreadPool::executeInParallelNoWait(ra);
}

void PrimeMonitor::terminateJobs() {
  m_lock.wait();
  for(size_t i = 0; i < m_jobs.size(); i++) {
    m_jobs[i]->stopSearch();
  }
  m_lock.notify();
  while(m_runningCount > 0) {
    Sleep(500);
  }
}

Array<BigInt> PrimeMonitor::getResult() const {
  Array<BigInt> result;
  m_lock.wait();
  result = m_result;
  m_lock.notify();
  return result;
}


PrimeSearcher::PrimeSearcher(PrimeMonitor *mon, int id)
: m_mon( *mon)
, m_id(   id )
, m_pool(BigRealResourcePool::fetchDigitPool())
{
}

PrimeSearcher::~PrimeSearcher() {
  m_terminated.wait();
  BigRealResourcePool::releaseDigitPool(m_pool);
}

UINT PrimeSearcher::run() {
  m_terminated.wait();
  m_mon.m_runningCount++;
  SETTHREADDESCRIPTION(format(_T("PrimeSearcher %d"), m_id));
  const int           digitCount = m_mon.m_digitCount;
  MillerRabinHandler *handler    = m_mon.m_handler;
  JavaRandom rnd;
  rnd.randomize();
  try {
    for(;;) {
      BigInt n = randomOddInteger(digitCount, rnd, m_pool);
      const Sieve sv(n);
      int sieveCount = 0;
      for(int i = 1;; n += BigReal::_2, i++) {
        int smallFactor;
        if(sv.hasSmallFactor(n,smallFactor)) {
          sieveCount++;
          if(handler) handler->handleData(MillerRabinCheck(m_id, n, format(_T("has factor %6u. (Sieved number:%d)"),smallFactor, sieveCount),false));
          continue;
        }
        if(MRisprime(n, m_id, handler)) {
          m_mon.putPrime(n);
          break;
        }
      }
    }
  } catch(...) {
    // ignore
  }

  m_mon.m_runningCount--;
  m_terminated.notify();
  return 0;
}

Array<BigInt> findRandomPrimes(int count, int digitCount, int threadCount, DigitPool *pool, MillerRabinHandler *handler) {
  if(pool == nullptr) pool = DEFAULT_DIGITPOOL;
  PrimeMonitor m(digitCount, threadCount, pool, handler);
  m.startJobs();
  while(m.getPrimesFound() < count) {
    Sleep(1000);
  }
  m.terminateJobs();
  Array<BigInt> resultArray = m.getResult();
  return resultArray;
}

BigInt findRandomPrime(int digitCount, int threadCount, DigitPool *pool, MillerRabinHandler *handler) {
  Array<BigInt> pa = findRandomPrimes(1, digitCount, threadCount, pool, handler);
  return pa[0];
}
