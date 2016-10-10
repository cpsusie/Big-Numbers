#include "pch.h"
#include <BitSet.h>
#include <CPUInfo.h>
#include <SynchronizedQueue.h>
#include <Math/MRisprime.h>

static BigInt randomOddInteger(int digits, Random &rnd, DigitPool *pool) {
  BigInt n = randomInteger(digits, &rnd, pool);
  if(even(n)) ++n;
  return n;
}

#define SIEVESIZE 30000
class SmallPrimeSet {
private:
  static BitSet m_primeSet;
public:
  SmallPrimeSet();
  static BitSet &getPrimeSet() {
    return m_primeSet;
  }
};

BitSet SmallPrimeSet::m_primeSet = BitSet(SIEVESIZE);

SmallPrimeSet::SmallPrimeSet() {
  m_primeSet.clear();
  m_primeSet.invert();
  m_primeSet.remove(0,1);
  for(int i = 2; i < SIEVESIZE; i++) {
    if(m_primeSet.contains(i)) {
      for(int j = i+i; j < SIEVESIZE; j+=i) {
        m_primeSet.remove(j);
      }
    }
  }
}

static SmallPrimeSet smallPrimeSet;

class Sieve {
private:
  const BigInt m_base;
  int           m_remainder[SIEVESIZE];
public:
  Sieve(const BigInt &n);
  bool hasSmallFactor(const BigInt &n, int &factor) const;
};

Sieve::Sieve(const BigInt &n) : m_base(n) {
  DigitPool *pool = n.getDigitPool();
  memset(m_remainder,0,sizeof(m_remainder));
  for(Iterator<size_t> it = SmallPrimeSet::getPrimeSet().getIterator(); it.hasNext(); ) {
    size_t p = it.next();
    BigInt r = BigInt(m_base % BigInt(p, pool));
    m_remainder[p] = getUlong(r);
  }
}

bool Sieve::hasSmallFactor(const BigInt &n, int &factor) const {
  BigInt d = BigInt(n - m_base);
  int diff = (int)getDouble(d);
  for(Iterator<size_t> it = SmallPrimeSet::getPrimeSet().getIterator(); it.hasNext(); ) {
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

class PrimeSearcher : public Thread {
private:
  const int           m_id;
  const int           m_digitCount;
  PrimeQueue         &m_queue;
  DigitPool          *m_pool;
  Random              m_rnd;
  MillerRabinHandler *m_handler;
  bool                m_deletePending;
public:
  PrimeSearcher(int id, int digitCount, PrimeQueue &queue, MillerRabinHandler *handler);
  ~PrimeSearcher();
  UINT run();
};

PrimeSearcher::PrimeSearcher(int id, int digitCount, PrimeQueue &queue, MillerRabinHandler *handler) 
: m_id(        id        )
, m_digitCount(digitCount)
, m_queue(     queue     )
, m_handler(   handler   )
{
  m_rnd.randomize();
  m_pool = BigRealThreadPool::fetchDigitPool();
  m_deletePending = false;
  setDeamon(true);
}

PrimeSearcher::~PrimeSearcher() {
  m_deletePending = true;
  for(int i = 0; i < 10; i++) {
    if(stillActive()) {
      Sleep(1000);
    }
  }
  if(stillActive()) {
    debugLog(_T("Cannot kill PrimeSearcher (id=%d)"), m_id);
  }
  BigRealThreadPool::releaseDigitPool(m_pool);
}

UINT PrimeSearcher::run() {
  try {
    BigInt n = randomOddInteger(m_digitCount, m_rnd, m_pool);
    const Sieve sv(n);
    for(int i = 1;;n += BIGREAL_2, i++) {
      if(m_deletePending) {
        break;
      }
      int smallFactor;
      if(sv.hasSmallFactor(n,smallFactor)) {
        if(m_handler != NULL) {
          m_handler->handleData(MillerRabinCheck(m_id, n, i, format(_T("has factor %u"),smallFactor)));
        }
        continue;
      }
      if(MRisprime(m_id, n, m_handler)) {
        m_queue.put(n);
        break;
      }
    }
  } catch(...) {
    // ignore
  }
  return 0;
}


BigInt findRandomPrime(int digitCount, int threadCount, DigitPool *pool, MillerRabinHandler *handler) {
  BigReal::resumeCalculation();
  redirectDebugLog();
  PrimeQueue resultQueue;
  const int processorCount = getProcessorCount();
  if(threadCount > processorCount) threadCount = processorCount;

  CompactArray<PrimeSearcher*> jobs;
  for(int i = 0; i < threadCount; i++) {
    jobs.add(new PrimeSearcher(i, digitCount, resultQueue, handler));
    jobs.last()->start();
  }
  const BigInt n = resultQueue.get();
  BigReal::terminateCalculation();

  for(int i = 0; i < threadCount; i++) {
    delete jobs[i];
  }

  jobs.clear();
  if(pool == NULL) pool = &DEFAULT_DIGITPOOL;
  return BigInt(n, pool);
}
