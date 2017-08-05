#include "pch.h"
#include <ByteFile.h>
#include <CompressFilter.h>

DEFINECLASSNAME(Pow2Cache);

#ifdef _DEBUG
size_t Pow2Cache::s_cacheHitCount     = 0;
size_t Pow2Cache::s_cacheRequestCount = 0;
#endif

#ifdef IS32BIT
#define CACHEFILENAME _T("c:\\temp\\Pow2Cachex86.dat")
#else
#define CACHEFILENAME _T("c:\\temp\\Pow2Cachex64.dat")
#endif

#define BITPERBRDIGIT (sizeof(BRDigitType)*8)

Pow2Cache::Pow2Cache() {
  m_state       = CACHE_EMPTY;
  m_updateCount = m_savedCount = 0;
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
    save(CACHEFILENAME);
    m_savedCount = m_updateCount;
    m_gate.signal();
  }
}

void Pow2Cache::clear() {
  m_gate.wait();
  for(Iterator<Entry<Pow2ArgumentKey, BigReal*> > it = getEntryIterator(); it.hasNext();) {
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
  if(m_state & (CACHE_LOADED|CACHE_LOADING)) {
    if(isLoaded()) {
      throwException(_T("%s:Not allowed when cache is loaded from file"), method);
    }
    m_state &= ~CACHE_EMPTY;
    ret = __super::put(key, v);
  } else {
    m_gate.wait();
    m_state &= ~CACHE_EMPTY;
    ret = __super::put(key, v);
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

BigReal **Pow2Cache::get(const Pow2ArgumentKey &key) {
  if(isLoaded()) {
    return __super::get(key);
  }
  m_gate.wait();
  BigReal **result = __super::get(key);
  m_gate.signal();
  return result;
}

void Pow2Cache::save(const String &fileName) {
  save(CompressFilter(ByteOutputFile(fileName)));
}

void Pow2Cache::load(const String &fileName) {
  load(DecompressFilter(ByteInputFile(fileName)));
}

void Pow2Cache::save(ByteOutputStream &s) const {
  const UINT capacity = (UINT)getCapacity();
  const UINT n        = (UINT)size();
#ifdef _DEBUG
  debugLog(_T("Saving Pow2Cache to %s. size:%lu, capacity:%lu\n"), CACHEFILENAME, n, capacity);
#endif
  const BYTE signaturByte = BITPERBRDIGIT;
  s.putByte(signaturByte);
  s.putBytes((BYTE*)&capacity, sizeof(capacity));
  s.putBytes((BYTE*)&n       , sizeof(n));
  for(Iterator<Entry<Pow2ArgumentKey, BigReal*> > it = getEntryIterator(); it.hasNext();) {
    const Entry<Pow2ArgumentKey, BigReal*> &e = it.next();
    e.getKey().save(s);
    e.getValue()->save(s);
  }
#ifdef _DEBUG
  debugLog(_T("Pow2Cache saved to %s\n"), CACHEFILENAME);
#endif
}

void Pow2Cache::load(ByteInputStream &s) {
  UINT capacity;
  UINT n;
  const BYTE signaturByte = s.getByte();
  if(signaturByte != BITPERBRDIGIT) {
    throwException(_T("Wrong bits/Digit in cache-file. Expected %d, got %d bits"), BITPERBRDIGIT, signaturByte);
  }
  s.getBytesForced((BYTE*)&capacity, sizeof(capacity));
  s.getBytesForced((BYTE*)&n       , sizeof(n));

#ifdef _DEBUG
  debugLog(_T("Loading Pow2Cache. size:%lu, capacity:%lu...\n"), n, capacity);
#endif
  setCapacity(capacity);
  m_state |= CACHE_LOADING;
  for(UINT i = 0; i < n; i++) {
    const Pow2ArgumentKey key(s);
    put(key, new BigReal(s, &DEFAULT_DIGITPOOL));
  }
  m_state = CACHE_LOADED;
#ifdef _DEBUG
  debugLog(_T("Pow2Cache loaded from %s\n"), CACHEFILENAME);
#endif
}

const BigReal &BigReal::pow2(int n, size_t digits) { // static
  const Pow2ArgumentKey key(n, digits);
#ifdef _DEBUG
  Pow2Cache::s_cacheRequestCount++;
#endif

  BigReal **result = s_pow2Cache.get(key);
  if(result == NULL) {
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
  } else {
#ifdef _DEBUG
    Pow2Cache::s_cacheHitCount++;
#endif
  }
  return **result;
}

void BigReal::loadPow2Cache() {
  s_pow2Cache.load();
}

void BigReal::savePow2Cache() {
  s_pow2Cache.save();
}

bool BigReal::hasPow2CacheFile() {
  return s_pow2Cache.hasCacheFile();
}

bool BigReal::pow2CacheChanged() {
  return s_pow2Cache.isChanged();
}
