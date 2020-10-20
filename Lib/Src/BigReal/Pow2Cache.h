#include "pch.h"
#include <CompactHashMap.h>

class Pow2ArgumentKey {
public:
  int    m_n;
  size_t m_digits;
  Pow2ArgumentKey() : m_n(0), m_digits(0) {
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
  friend class BigRealResourcePool;
private:
  FastSemaphore         m_lock;
  // All values, saved in cache, uses m_digitpool
  // cand m_workPool is used to calculate new values
  DigitPool            *m_digitPool, *m_workPool;

  size_t                m_updateCount, m_savedCount;
  void save(const String &fileName) const;
  void load(const String &fileName);
  void save(ByteOutputStream &s) const;
  void load(ByteInputStream  &s);
  void clear();
  Pow2Cache(DigitPool *digitPool, DigitPool *workPool);
  ~Pow2Cache() override;
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
  const BigReal &calculatePow2(int n, size_t digits);

  inline Pow2Cache &wait() { m_lock.wait(); return *this; }
  inline void notify()     { m_lock.notify(); }

  // return the one and only instance of Pow2Cache, with a lock
  static Pow2Cache &getInstance();
};

typedef Entry<Pow2ArgumentKey, const BigReal*> Pow2CacheEntry;
