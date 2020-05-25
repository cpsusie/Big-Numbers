#include "pch.h"

#if defined(TRACEPOW2CACHE)
#include <DebugLog.h>
#endif

#include <ByteFile.h>
#include <CompressFilter.h>
#include "ResourcePoolInternal.h"

#ifdef TRACEPOW2CACHE
#define LOGPOW2CACHE(...) debugLog(__VA_ARGS__)
#else
#define LOGPOW2CACHE(...)
#endif // TRACEPOW2CACHE

#ifdef TRACEPOW2CACHEHIT
#include <CallCounter.h>
static CallCounter pow2CacheHits(    _T("pow2CacheHits"    ));
static CallCounter pow2CacheRequests(_T("pow2CacheRequests"));
#define ADDCACHEHIT() pow2CacheHits.incr()
#define ADDCACJEREQ() pow2CacheRequests.incr()
#else
#define ADDCACHEHIT()
#define ADDCACHEREQ()
#endif // TRACEPOW2CACHEHIT

#define CACHEFILENAME _T("c:\\temp\\Pow2Cache.dat")

Pow2Cache &Pow2Cache::getInstance() { // static
  return BigRealResourcePool::getPow2Cache()->wait();
}

Pow2Cache::Pow2Cache(DigitPool *digitPool, DigitPool *workPool)
: m_digitPool(digitPool)
, m_workPool( workPool )
{
  m_digitPool->setName(_T("POW2CACHE"));
  m_workPool->setName( _T("POW2WORK"));
  m_updateCount = m_savedCount = 0;
//  load();
}

Pow2Cache::~Pow2Cache() {
  clear();
}

bool Pow2Cache::hasCacheFile() { // static
  return ACCESS(CACHEFILENAME, 0) == 0;
}

void Pow2Cache::load() {
  if(hasCacheFile()) {
    while(!isEmpty()) {
      clear();
    }
    setCapacity(33000);
    load(CACHEFILENAME);
    m_updateCount = m_savedCount = 0;
  }
}

void Pow2Cache::save() {
  if(isChanged()) {
    save(CACHEFILENAME);
    m_savedCount = m_updateCount;
  }
}

void Pow2Cache::clear() {
  for(Iterator<Pow2CacheEntry> it = getEntryIterator(); it.hasNext();) {
    BigReal *v = (BigReal*)it.next().getValue();
    SAFEDELETE(v);
  }
  __super::clear();
  m_updateCount++;
}

bool Pow2Cache::put(const Pow2ArgumentKey &key, BigReal * const &v) {
  assert(v->getDigitPool() == m_digitPool);
  const bool ret = __super::put(key, v);
  if(ret) {
    TRACE_NEW(v);
    m_updateCount++;
  } else {
    delete v; // NB not SAFEDELETE
  }
  return ret;
}

void Pow2Cache::save(const String &fileName) const {
  save(CompressFilter(ByteOutputFile(fileName)));
}

void Pow2Cache::load(const String &fileName) {
  load(DecompressFilter(ByteInputFile(fileName)));
}

static inline Packer &operator<<(Packer &p, const Pow2ArgumentKey &key) {
  return p << key.m_n << key.m_digits;
}

static inline Packer &operator>>(Packer &p, Pow2ArgumentKey &key) {
  return p >> key.m_n >> key.m_digits;
}

void Pow2Cache::save(ByteOutputStream &s) const {
  const UINT capacity = (UINT)getCapacity();
  const UINT n        = (UINT)size();
  LOGPOW2CACHE(_T("Saving Pow2Cache to %s. size:%lu, capacity:%lu\n"), CACHEFILENAME, n, capacity);
  Packer p;
  p << capacity << n;
  for(Iterator<Pow2CacheEntry> it = getEntryIterator(); it.hasNext();) {
    const Pow2CacheEntry &e = it.next();
    p << e.getKey() << *e.getValue();
  }
  p.write(s);
  p.writeEos(s);
  LOGPOW2CACHE(_T("Pow2Cache saved to %s\n"), CACHEFILENAME);
}

void Pow2Cache::load(ByteInputStream &s) {
  UINT capacity;
  UINT n;
  Packer p;
  p.read(s);
  p >> capacity >> n;
  if(n > capacity) {
    capacity = 65993;
  }
  LOGPOW2CACHE(_T("Loading Pow2Cache. size:%lu, capacity:%lu...\n"), n, capacity);
  setCapacity(capacity);
  BigReal tmp(m_workPool);
  for(UINT i = 0; i < n; i++) {
    Pow2ArgumentKey key;
    p >> key >> tmp;
    put(key, new BigReal(tmp, m_digitPool));
  }
  LOGPOW2CACHE(_T("Pow2Cache loaded from %s\n"), CACHEFILENAME);
}

const BigReal &Pow2Cache::calculatePow2(int n, size_t digits) {
  const Pow2ArgumentKey key(n, digits);

  ADDCACHEREQ();
  const BigReal **result = get(key);
  if(result != NULL) {
    ADDCACHEHIT();
    return **result;
  } else {
    if(digits != 0) {
      result = get(Pow2ArgumentKey(n,0));
    }
    if(result != NULL) {
      put(key, new BigReal(::cut(**result, digits,m_workPool),m_digitPool));
    } else if(n == 0) {
      put(key, new BigReal(m_digitPool->_1()));              // 2^0 == 1
    } else {
      BigReal tmp(m_workPool);
      if(isEven(n)) {                                   // n even
        const BigReal &t = calculatePow2(n/2,digits);
        if(digits == 0) {
          tmp = prod(t, t, m_workPool->_0(),m_workPool);
        } else {
          tmp = rProd(t, t, digits, m_workPool).rRound(digits); // 2^n = pow2(n/2)^2
        }
      } else if(n < 0) {                                      // n odd && (n < 0)
        tmp = calculatePow2(n + 1, digits);
        tmp.divide2();
        if(digits) tmp.rRound(digits);
      } else {                                                 // n odd && (n > 0)
        tmp = calculatePow2(n - 1, digits);
        tmp.multiply2();
        if(digits) tmp.rRound(digits);
      }
      put(key, new BigReal(tmp, m_digitPool));
    }
    result = get(key);
  }
  return **result;
}

const BigReal &BigReal::pow2(int n, size_t digits) { // static
  const Pow2ArgumentKey key(n, digits);

  Pow2Cache &cache = Pow2Cache::getInstance();

  const BigReal **result = cache.get(key);
  if(result != NULL) {
    ADDCACHEREQ();
    ADDCACHEHIT();
  } else {
    try {
      const BigReal &p2 = cache.calculatePow2(n, digits);
      result = cache.get(key);
    } catch(...) {
      cache.notify();
      throw;
    }
  }
  cache.notify();
  return **result;
}

void BigReal::pow2CacheLoad() { // static
  Pow2Cache &cache = Pow2Cache::getInstance();
  cache.load();
  cache.notify();
}

void BigReal::pow2CacheSave() { // static
  Pow2Cache &cache = Pow2Cache::getInstance();
  cache.save();
  cache.notify();
}

bool BigReal::pow2CacheHasFile() { // static
  return Pow2Cache::hasCacheFile();
}

bool BigReal::pow2CacheChanged() { // static
  Pow2Cache &cache = Pow2Cache::getInstance();
  const bool result = cache.isChanged();
  cache.notify();
  return result;
}
