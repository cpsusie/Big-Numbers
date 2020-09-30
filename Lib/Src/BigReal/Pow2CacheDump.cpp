#include "pch.h"
#include "Pow2Cache.h"

class ArrayEntry : public Pow2ArgumentKey {
private:
  const BigReal *m_v;
public:
  ArrayEntry() : m_v(NULL) {
  }
  ArrayEntry(const Pow2CacheEntry &e) : Pow2ArgumentKey(e.getKey()), m_v(e.getValue()) {
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
  for(Iterator<Pow2CacheEntry> it = ((Pow2Cache*)cache)->getIterator(); it.hasNext();) {
    const Pow2CacheEntry &e = it.next();
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

void BigReal::pow2CacheDump() { // static
  Pow2Cache &cache = Pow2Cache::getInstance();
  return cache.dump();
  cache.notify();
}
