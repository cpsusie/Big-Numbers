#include "pch.h"
#include <DebugLog.h>
#include <ByteFile.h>
#include <CompressFilter.h>

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

#define SETSTATEFLAG(f) m_state |=  (f)
#define CLRSTATEFLAG(f) m_state &= ~(f)


Pow2Cache::Pow2Cache() {
  m_state       = CACHE_EMPTY;
  m_updateCount = m_savedCount = 0;
  load();
}

Pow2Cache::~Pow2Cache() {
  clear();
}

bool Pow2Cache::hasCacheFile() const {
  return ACCESS(CACHEFILENAME, 0) == 0;
}

void Pow2Cache::load() {
  if(hasCacheFile()) {
    m_gate.wait();
    while(!isEmpty()) {
      m_gate.signal();
      clear();
      m_gate.wait();
    }
    setCapacity(33000);
    load(CACHEFILENAME);
    m_updateCount = m_savedCount = 0;
    m_gate.signal();
  }
}

void Pow2Cache::save() {
  if(isChanged()) {
    m_gate.wait();
    SETSTATEFLAG(CACHE_SAVING);
    save(CACHEFILENAME);
    m_savedCount = m_updateCount;
    CLRSTATEFLAG(CACHE_SAVING);
    m_gate.signal();
  }
}

typedef Entry<Pow2ArgumentKey, BigReal*> CacheEntry;

class ArrayEntry : public Pow2ArgumentKey {
private:
  const BigReal *m_v;
public:
  ArrayEntry() : m_v(NULL) {
  }
  ArrayEntry(const CacheEntry &e) : Pow2ArgumentKey(e.getKey()), m_v(e.getValue()) {
  }
  String toString() const {
    return format(_T("%s:%s"), __super::toString().cstr(), FullFormatBigReal(*m_v).toString().cstr());
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
  m_gate.wait();
  CacheArray a(this);
  a.sort(entryCmp);
  redirectDebugLog();
  for(Iterator<ArrayEntry> it = a.getIterator(); it.hasNext();) {
    debugLog(_T("%s\n"), it.next().toString().cstr());
  }
  a.clear();
  m_gate.signal();
}

void Pow2Cache::clear() {
  m_gate.wait();
  for(Iterator<CacheEntry> it = getEntryIterator(); it.hasNext();) {
    BigReal *v = it.next().getValue();
    SAFEDELETE(v);
  }
  __super::clear();
  m_state = CACHE_EMPTY;
  m_updateCount++;
  m_gate.signal();
}

bool Pow2Cache::put(const Pow2ArgumentKey &key, BigReal * const &v) {
  DEFINEMETHODNAME;
  bool ret;
  if(m_state & (CACHE_LOADED|CACHE_LOADING|CACHE_SAVING)) {
    if(m_state & (CACHE_LOADED | CACHE_SAVING)) {
      throwException(_T("%s:Not allowed when cache is loaded from file or saving to file"), method);
    }
    if(ret = __super::put(key, v)) {
      CLRSTATEFLAG(CACHE_EMPTY);
    }
  } else {
    m_gate.wait();
    if(ret = __super::put(key, v)) {
      CLRSTATEFLAG(CACHE_EMPTY);
    }
    m_gate.signal();
  }
  if(ret) {
    TRACE_NEW(v);
    m_updateCount++;
  } else {
    delete v; // NB not SAFEDELETE
  }
  return ret;
}

BigReal **Pow2Cache::get(const Pow2ArgumentKey &key) const {
  if(m_state & (CACHE_LOADED | CACHE_SAVING)) {
    return __super::get(key);
  }
  m_gate.wait();
  BigReal **result = __super::get(key);
  m_gate.signal();
  return result;
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

  LOGPOW2CACHE(_T("Loading Pow2Cache. size:%lu, capacity:%lu...\n"), n, capacity);
  setCapacity(capacity);
  SETSTATEFLAG(CACHE_LOADING);
  for(UINT i = 0; i < n; i++) {
    Pow2ArgumentKey key;
    BigReal *v = new ConstBigReal(0);
    p >> key >> *v;
    put(key, v);
  }
  CLRSTATEFLAG(CACHE_LOADING);
  SETSTATEFLAG(CACHE_LOADED);
  LOGPOW2CACHE(_T("Pow2Cache loaded from %s\n"), CACHEFILENAME);
}

const BigReal &BigReal::pow2(int n, size_t digits) { // static
  const Pow2ArgumentKey key(n, digits);

  ADDCACHEREQ();
  BigReal **result = s_pow2Cache.get(key);
  if(result != NULL) {
    ADDCACHEHIT();
  } else {
    if(digits != 0) {
      result = s_pow2Cache.get(Pow2ArgumentKey(n,0));
    }
    if(result != NULL) {
      s_pow2Cache.put(key, new ConstBigReal(::cut(**result, digits)));
    } else if(n == 0) {
      s_pow2Cache.put(key, new ConstBigReal(BIGREAL_1)); // 2^0 == 1
    } else if((n & 1) == 0) {                           // n even
      ConstBigReal t = pow2(n/2,digits);
      s_pow2Cache.put(key, new ConstBigReal((digits == 0) ? (t*t) : rProd(t,t,digits)));           // 2^n = pow2(n/2)^2
    } else if(n < 0) {                                  // n odd && < 0
      s_pow2Cache.put(key, new ConstBigReal((digits == 0) ? pow2(n+1,digits) * BIGREAL_HALF: rProd(pow2(n+1,digits),BIGREAL_HALF, digits)));
    } else {                                            // n odd && > 0
      s_pow2Cache.put(key, new ConstBigReal((digits == 0) ? pow2(n-1,digits) * BIGREAL_2   : rProd(pow2(n-1,digits),BIGREAL_2   , digits)));
    }
    result = s_pow2Cache.get(key);
  }
  return **result;
}

void BigReal::pow2CacheLoad() { // static
  s_pow2Cache.load();
}

void BigReal::pow2CacheSave() { // static
  s_pow2Cache.save();
}

bool BigReal::pow2CacheHasFile() { // static
  return s_pow2Cache.hasCacheFile();
}

bool BigReal::pow2CacheChanged() { // static
  return s_pow2Cache.isChanged();
}

void BigReal::pow2CacheDump() { // static
  return s_pow2Cache.dump();
}
