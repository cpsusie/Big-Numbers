#include "pch.h"
#include <DebugLog.h>
#include <CompactHashMap.h>
#include <ByteFile.h>
#include <CompressFilter.h>
#include <SingletonFactory.h>

class Pow2ArgumentKey {
public:
  int    m_n;
  size_t m_digits;
  Pow2ArgumentKey() {
  }
  inline Pow2ArgumentKey(int n, size_t digits) : m_n(n), m_digits(digits) {
  }
  inline ULONG hashCode() const {
    return m_n * 23 + sizetHash(m_digits);
  }
  inline bool operator==(const Pow2ArgumentKey &k) const {
    return (m_n == k.m_n) && (m_digits == k.m_digits);
  }
  String toString() const {
    return format(_T("(%6d,%3zu)"), m_n, m_digits);
  }
};

class Pow2Cache : public CompactHashMap<Pow2ArgumentKey, const BigReal*> {
  friend class Pow2CacheFactory;
private:
  // All values, saved in cache, uses this digitpool
  FastSemaphore         m_lock;
  DigitPool            *m_digitPool;
  size_t                m_updateCount, m_savedCount;

  void save(const String &fileName) const;
  void load(const String &fileName);
  void save(ByteOutputStream &s) const;
  void load(ByteInputStream  &s);
  void clear();
  Pow2Cache();
  ~Pow2Cache();
  Pow2Cache(const Pow2Cache &src);            // not implemented
  Pow2Cache &operator=(const Pow2Cache &src); // not implemented
public:
  bool put(const Pow2ArgumentKey &key, BigReal * const &v);
  inline bool isChanged() const {
    return m_updateCount != m_savedCount;
  }
  static bool hasCacheFile();
  void load();
  void save();
  void dump() const;
  const BigReal &calculatePow2(int n, size_t digits, DigitPool *workPool);

  inline void wait()   { m_lock.wait(); }
  inline void notify() { m_lock.notify(); }

  // return the one and only instance of Pow2Cache, with a lock
  static Pow2Cache &getInstance();
};

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

Pow2Cache::Pow2Cache() {
  m_digitPool   = BigRealResourcePool::fetchDigitPool(false, 0);
  m_digitPool->setName(_T("POW2CACHE"));
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

typedef Entry<Pow2ArgumentKey, const BigReal*> CacheEntry;

class ArrayEntry : public Pow2ArgumentKey {
private:
  const BigReal *m_v;
public:
  ArrayEntry() : m_v(NULL) {
  }
  ArrayEntry(const CacheEntry &e) : Pow2ArgumentKey(e.getKey()), m_v(e.getValue()) {
  }
  String toString() const {
    return format(_T("%s:%s"), __super::toString().cstr(), ::toString(*(FullFormatBigReal*)m_v).cstr());
  }
};

class CacheArray : public CompactArray<ArrayEntry> {
public:
  CacheArray(const Pow2Cache *cache);
};

CacheArray::CacheArray(const Pow2Cache *cache) : CompactArray<ArrayEntry>(cache->size()) {
  for(Iterator<CacheEntry> it = ((Pow2Cache*)cache)->getEntryIterator(); it.hasNext();) {
    const CacheEntry &e = it.next();
    add(e);
  }
}

static int entryCmp(const ArrayEntry &e1, const ArrayEntry &e2) {
  const int c = e1.m_n - e2.m_n;
  if(c) return c;
  return sign((intptr_t)e1.m_digits - (intptr_t)e2.m_digits);
}

void Pow2Cache::dump() const {
  CacheArray a(this);
  a.sort(entryCmp);
  redirectDebugLog();
  for(Iterator<ArrayEntry> it = a.getIterator(); it.hasNext();) {
    debugLog(_T("%s\n"), it.next().toString().cstr());
  }
  a.clear();
  CompactIntArray lengthArray = getLength();
  debugLog(_T("size:%zu, capacity:%zu\nLength:%s\n")
          ,size(), getCapacity(), lengthArray.toStringBasicType().cstr());

}

void Pow2Cache::clear() {
  for(Iterator<CacheEntry> it = getEntryIterator(); it.hasNext();) {
    BigReal *v = (BigReal*)it.next().getValue();
    SAFEDELETE(v);
  }
  __super::clear();
  m_updateCount++;
}

bool Pow2Cache::put(const Pow2ArgumentKey &key, BigReal * const &v) {
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
  for(Iterator<CacheEntry> it = getEntryIterator(); it.hasNext();) {
    const CacheEntry &e = it.next();
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
  DigitPool *workPool = NULL;
  try {
    LOGPOW2CACHE(_T("Loading Pow2Cache. size:%lu, capacity:%lu...\n"), n, capacity);
    setCapacity(capacity);
    workPool = BigRealResourcePool::fetchDigitPool();
    BigReal tmp(workPool);
    for(UINT i = 0; i < n; i++) {
      Pow2ArgumentKey key;
      p >> key >> tmp;
      put(key, new BigReal(tmp, m_digitPool));
    }
    LOGPOW2CACHE(_T("Pow2Cache loaded from %s\n"), CACHEFILENAME);
  } catch (...) {
    if(workPool) {
      BigRealResourcePool::releaseDigitPool(workPool); workPool = NULL;
    }
    throw;
  }
  BigRealResourcePool::releaseDigitPool(workPool);
}

DEFINESINGLETONFACTORY(Pow2Cache);

Pow2Cache &Pow2Cache::getInstance() { // static
  static Pow2CacheFactory factory;
  Pow2Cache &cache = factory.getInstance();
  cache.wait();
  return cache;
}

const BigReal &Pow2Cache::calculatePow2(int n, size_t digits, DigitPool *workPool) {
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
      put(key, new BigReal(::cut(**result, digits,workPool),m_digitPool));
    } else if(n == 0) {
      put(key, new BigReal(m_digitPool->_1()));              // 2^0 == 1
    } else {
      BigReal tmp(workPool);
      if(isEven(n)) {                                   // n even
        const BigReal &t = calculatePow2(n/2,digits,workPool);
        if(digits == 0) {
          tmp = prod(t, t, workPool->_0(),workPool);
        } else {
          tmp = rProd(t, t, digits, workPool).rRound(digits); // 2^n = pow2(n/2)^2
        }
      } else if(n < 0) {                                      // n odd && (n < 0)
        tmp = calculatePow2(n + 1, digits, workPool);
        tmp.divide2();
        if(digits) tmp.rRound(digits);
      } else {                                                 // n odd && (n > 0)
        tmp = calculatePow2(n - 1, digits, workPool);
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
    DigitPool *pool = NULL;
    try {
      pool = BigRealResourcePool::fetchDigitPool();
      const BigReal &p2 = cache.calculatePow2(n, digits, pool);
      BigRealResourcePool::releaseDigitPool(pool);
      pool = NULL;
      result = cache.get(key);
    } catch (...) {
      if(pool) {
        BigRealResourcePool::releaseDigitPool(pool);
        pool = NULL;
      }
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

void BigReal::pow2CacheDump() { // static
  Pow2Cache &cache = Pow2Cache::getInstance();
  return cache.dump();
  cache.notify();
}
