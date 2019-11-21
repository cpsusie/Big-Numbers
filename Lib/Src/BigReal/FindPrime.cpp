#include "pch.h"
#include <BitSet.h>
#include <CPUInfo.h>
#include <DebugLog.h>
#include <SynchronizedQueue.h>
#include <Math/MRisprime.h>

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
    m_remainder[p] = getUint(m_base % bp);
  }
}

bool Sieve::hasSmallFactor(const BigInt &n, int &factor) const {
  const int diff = getInt(n - m_base);
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

typedef SynchronizedQueue<BigInt> PrimeQueue;

#define PSST_ALLOCATED   0x01
#define PSST_RUNNING     0x02
#define PSST_STOPPENDING 0x04
#define PSST_TERMINATED  0x08

class PrimeSearcher : public Thread {
private:
  const int           m_id;
  const int           m_digitCount;
  PrimeQueue         &m_queue;
  DigitPool          *m_pool;
  JavaRandom          m_rnd;
  MillerRabinHandler *m_handler;
  Semaphore           m_gate;
  BYTE                m_flags;
  void modifyFlags(BYTE add, BYTE remove) {
    m_gate.wait();
    m_flags |= add;
    m_flags &= ~remove;
    m_gate.notify();
  }
public:
  PrimeSearcher(int id, int digitCount, PrimeQueue &queue, MillerRabinHandler *handler);
  ~PrimeSearcher();
  UINT run();
  inline BYTE getFlags() const {
    return m_flags;
  }
  inline void setStopPending() {
    modifyFlags(PSST_STOPPENDING, 0);
    m_pool->terminatePoolCalculation();
  }
  inline bool isRunning() const {
    return (m_flags & PSST_RUNNING) != 0;
  }
};

PrimeSearcher::PrimeSearcher(int id, int digitCount, PrimeQueue &queue, MillerRabinHandler *handler)
: m_id(        id        )
, m_digitCount(digitCount)
, m_queue(     queue     )
, m_handler(   handler   )
, m_pool(BigRealResourcePool::fetchDigitPool())
, m_flags(PSST_ALLOCATED   )
{
  m_rnd.randomize();
  setDeamon(true);
}

PrimeSearcher::~PrimeSearcher() {
  assert(!isRunning());
  for(int i = 0; i < 10; i++) {
    if(stillActive()) {
      Sleep(1000);
    }
  }
  if(stillActive()) {
    debugLog(_T("Cannot kill PrimeSearcher (id=%d)"), m_id);
  }
  BigRealResourcePool::releaseDigitPool(m_pool);
}

#define CHECKTERMINATE() if(getFlags() & PSST_STOPPENDING) goto Terminate;
UINT PrimeSearcher::run() {
  modifyFlags(PSST_RUNNING,0);
  try {
    for(;;) {
      CHECKTERMINATE();
      BigInt n = randomOddInteger(m_digitCount, m_rnd, m_pool);
      CHECKTERMINATE();
      const Sieve sv(n);
      int sieveCount = 0;
      CHECKTERMINATE();
      for(int i = 1;; n += BigReal::_2, i++) {
        CHECKTERMINATE();
        int smallFactor;
        if(sv.hasSmallFactor(n,smallFactor)) {
          CHECKTERMINATE();
          sieveCount++;
          if(m_handler) m_handler->handleData(MillerRabinCheck(m_id, n, format(_T("has factor %6u. (Sieved number:%d)"),smallFactor, sieveCount),false));
          continue;
        }
        if(MRisprime(n, m_id, m_handler)) {
          CHECKTERMINATE();
          m_queue.put(n);
          break;
        }
      }
    }
  } catch(...) {
    // ignore
  }
Terminate:
  modifyFlags(PSST_TERMINATED, PSST_RUNNING|PSST_STOPPENDING);
  return 0;
}

class PRMonitor {
private:
  mutable Semaphore            m_gate;
  mutable PrimeQueue           m_resultQueue;
  CompactArray<PrimeSearcher*> m_jobs;
  const int                    m_digitCount;
  DigitPool                   *m_digitPool;
public:
  PRMonitor(int digitCount, int threadCount, DigitPool *pool, MillerRabinHandler *handler);
  ~PRMonitor();
  void startJobs();
  void terminateJobs();
  int  getRunningCount() const;
  inline int getPrimesFound() const {
    return m_resultQueue.size();
  }
  Array<BigInt> getResult() const;
};

PRMonitor::PRMonitor(int digitCount, int threadCount, DigitPool *digitPool, MillerRabinHandler *handler)
: m_digitCount(digitCount)
, m_digitPool(digitPool)
{
  threadCount = max(1, threadCount);
  const int processorCount = getProcessorCount();
  threadCount = min(threadCount,processorCount);
  m_gate.wait();
  for(int i = 0; i < threadCount; i++) {
    PrimeSearcher *ps = new PrimeSearcher(i, digitCount, m_resultQueue, handler); TRACE_NEW(ps);
    m_jobs.add(ps);
  }
  m_gate.notify();
}

PRMonitor::~PRMonitor() {
  terminateJobs();
  m_gate.wait();
  while(!m_resultQueue.isEmpty()) {
    m_resultQueue.get();
  }
  for(size_t i = 0; i < m_jobs.size(); i++) {
    SAFEDELETE(m_jobs[i]);
  }
  m_jobs.clear();
  m_gate.notify();
}

void PRMonitor::startJobs() {
  m_gate.wait();
  for(size_t i = 0; i < m_jobs.size(); i++) {
    m_jobs[i]->start();
  }
  m_gate.notify();
}

void PRMonitor::terminateJobs() {
  if(getRunningCount() > 0) {
    m_gate.wait();
    for(size_t i = 0; i < m_jobs.size(); i++) {
      m_jobs[i]->setStopPending();
    }
    m_gate.notify();
    while(getRunningCount() > 0) {
      Sleep(500);
    }
  }
}

int PRMonitor::getRunningCount() const {
  int count = 0;
  m_gate.wait();
  for(size_t i = 0; i < m_jobs.size(); i++) {
    if(m_jobs[i]->isRunning()) {
      count++;
    }
  }
  m_gate.notify();
  return count;
}

Array<BigInt> PRMonitor::getResult() const {
  Array<BigInt> result;
  m_gate.wait();
  while(!m_resultQueue.isEmpty()) {
    result.add(BigInt(m_resultQueue.get(), m_digitPool));
  }
  m_gate.notify();
  return result;
}

Array<BigInt> findRandomPrimes(int count, int digitCount, int threadCount, DigitPool *pool, MillerRabinHandler *handler) {
  if(pool == NULL) pool = DEFAULT_DIGITPOOL;
  PRMonitor m(digitCount, threadCount, pool, handler);
  m.startJobs();
  while(m.getPrimesFound() < count) {
    Sleep(1000);
  }
  m.terminateJobs();
  Array<BigInt> resultArray = m.getResult();
  return resultArray;
}
