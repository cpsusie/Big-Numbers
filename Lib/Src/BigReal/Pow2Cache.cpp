#include "pch.h"
#include <ByteFile.h>
#include <CompressFilter.h>

#ifdef _DEBUG
unsigned int Pow2Cache::s_cacheHitCount     = 0;
unsigned int Pow2Cache::s_cacheRequestCount = 0;
#endif

Pow2Cache::Pow2Cache() {
  m_loaded = false;
  m_gate.wait();
  if(isEmpty()) {
    setCapacity(33000);
    load(_T("c:\\temp\\Pow2Cache.dat"));
  }
  m_gate.signal();
}

Pow2Cache::~Pow2Cache() {
//    save(_T("c:\\temp\\Pow2Cache.dat"));
  m_gate.wait();
#ifdef _DEBUG
  debugLog(_T("CacheHits/CacheRequest:(%lu/%lu) =%.2lf%%\n"), s_cacheHitCount, s_cacheRequestCount, PERCENT(s_cacheHitCount, s_cacheRequestCount));
#endif
  clear();
  m_gate.signal();
}

void Pow2Cache::clear() {
  for(Iterator<Entry<Pow2ArgumentKey, BigReal*> > it = getEntryIterator(); it.hasNext();) {
    delete it.next().getValue();
  }
  CompactHashMap<Pow2ArgumentKey, BigReal*>::clear();
}

void Pow2Cache::save(const String &fileName) {
  save(CompressFilter(ByteOutputFile(fileName)));
}

void Pow2Cache::load(const String &fileName) {
  load(DecompressFilter(ByteInputFile(fileName)));
  m_loaded = true;
}

void Pow2Cache::save(ByteOutputStream &s) const {
  const UINT capacity = (UINT)getCapacity();
  const UINT n        = (UINT)size();
  debugLog(_T("saving Pow2Cache. size:%lu, capacity:%lu\n"), n, capacity);
  s.putBytes((BYTE*)&capacity, sizeof(capacity));
  s.putBytes((BYTE*)&n       , sizeof(n));
  for(Iterator<Entry<Pow2ArgumentKey, BigReal*> > it = getEntryIterator(); it.hasNext();) {
    Entry<Pow2ArgumentKey, BigReal*> &e = it.next();
    e.getKey().save(s);
    e.getValue()->save(s);
  }
}

void Pow2Cache::load(ByteInputStream &s) {
  clear();
  UINT capacity;
  UINT n;

  s.getBytesForced((BYTE*)&capacity, sizeof(capacity));
  s.getBytesForced((BYTE*)&n       , sizeof(n));

#ifdef _DEBUG
  debugLog(_T("loading Pow2Cache. size:%lu, capacity:%lu..."), n, capacity);
#endif
  setCapacity(capacity);
  for(UINT i = 0; i < n; i++) {
    Pow2ArgumentKey key(s);
    put(key, new BigReal(s, &DEFAULT_DIGITPOOL));
  }
#ifdef _DEBUG
  debugLog(_T("Pow2Cache loaded\n"));
#endif
}

const BigReal &BigReal::pow2(int n, unsigned int digits) { // static
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
