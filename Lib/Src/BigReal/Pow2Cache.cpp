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
  m_state     = CACHE_EMPTY;
  m_startSize = 0;
  if (ACCESS(CACHEFILENAME, 0) == 0) {
    m_gate.wait();
    if (isEmpty()) {
      setCapacity(33000);
      load(CACHEFILENAME);
    }
    m_startSize = size();
    m_gate.signal();
  }
}

Pow2Cache::~Pow2Cache() {
  if (size() != m_startSize) {
    save(CACHEFILENAME);
    m_gate.wait();
#ifdef _DEBUG
    debugLog(_T("CacheHits/CacheRequest:(%s/%s) =%.2lf%%\n"), format1000(s_cacheHitCount).cstr(), format1000(s_cacheRequestCount).cstr(), PERCENT(s_cacheHitCount, s_cacheRequestCount));
#endif
    clear();
    m_gate.signal();
  }
}

void Pow2Cache::clear() {
  for(Iterator<Entry<Pow2ArgumentKey, BigReal*> > it = getEntryIterator(); it.hasNext();) {
    delete it.next().getValue();
  }
  CompactHashMap<Pow2ArgumentKey, BigReal*>::clear();
  m_state = CACHE_EMPTY;
}

bool Pow2Cache::put(const Pow2ArgumentKey &key, BigReal * const &v) {
  DEFINEMETHODNAME;
  if (m_state & (CACHE_LOADED|CACHE_LOADING)) {
    if (isLoaded()) {
      throwMethodException(s_className, method, _T("Not allowed when cache is loaded from file"));
    }
    m_state &= ~CACHE_EMPTY;
    return ((CompactHashMap<Pow2ArgumentKey, BigReal*>*)this)->put(key, v);
  }
  else {
    m_gate.wait();
    m_state &= ~CACHE_EMPTY;
    const bool ret = ((CompactHashMap<Pow2ArgumentKey, BigReal*>*)this)->put(key, v);
    m_gate.signal();
    return ret;
  }
}

BigReal **Pow2Cache::get(const Pow2ArgumentKey &key) {
  if (isLoaded()) {
    return ((CompactHashMap<Pow2ArgumentKey, BigReal*>*)this)->get(key);
  }
  m_gate.wait();
  BigReal **result = ((CompactHashMap<Pow2ArgumentKey, BigReal*>*)this)->get(key);
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
  debugLog(_T("Saving Pow2Cache to %s. size:%lu, capacity:%lu\n"), CACHEFILENAME, n, capacity);
  const unsigned char signaturByte = BITPERBRDIGIT;
  s.putByte(signaturByte);
  s.putBytes((BYTE*)&capacity, sizeof(capacity));
  s.putBytes((BYTE*)&n       , sizeof(n));
  for(Iterator<Entry<Pow2ArgumentKey, BigReal*> > it = getEntryIterator(); it.hasNext();) {
    Entry<Pow2ArgumentKey, BigReal*> &e = it.next();
    e.getKey().save(s);
    e.getValue()->save(s);
  }
#ifdef _DEBUG
  debugLog(_T("Pow2Cache saved to %s\n"), CACHEFILENAME);
#endif
}

void Pow2Cache::load(ByteInputStream &s) {
  clear();
  UINT capacity;
  UINT n;
  const unsigned char signaturByte = s.getByte();
  if (signaturByte != BITPERBRDIGIT) {
    throwException(_T("Wrong bits/Digit in cache-file. Expected %d, got %d bits"), BITPERBRDIGIT, signaturByte);
  }
  s.getBytesForced((BYTE*)&capacity, sizeof(capacity));
  s.getBytesForced((BYTE*)&n       , sizeof(n));

#ifdef _DEBUG
  debugLog(_T("Loading Pow2Cache. size:%lu, capacity:%lu..."), n, capacity);
#endif
  setCapacity(capacity);
  m_state |= CACHE_LOADING;
  for(UINT i = 0; i < n; i++) {
    Pow2ArgumentKey key(s);
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
