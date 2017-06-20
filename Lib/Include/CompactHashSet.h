#pragma once

#include <Set.h>

#pragma pack(push,1)

#include "CompactKeyType.h"
#include "CompactElementPool.h"

template <class K> class SetEntry {
public:
  K m_key;
};

template <class K> class CompactHashElement : public SetEntry<K> {
public:
  CompactHashElement<K> *m_next;
};

#pragma pack(pop)

// Assume K has public member-function ULONG hashCode() const...
// and bool operator==(const K &) defined
template <class K> class CompactHashSet {
private:
  size_t                                     m_size;
  size_t                                     m_capacity;
  CompactHashElement<K>                    **m_buffer;
  CompactElementPool<CompactHashElement<K> > m_elementPool;
  UINT64                                     m_updateCount;

  CompactHashElement<K> **allocateBuffer(size_t capacity) const {
    CompactHashElement<K> **result = capacity ? new CompactHashElement<K>*[capacity] : NULL;
    if(capacity) {
      memset(result, 0, sizeof(result[0])*capacity);
    }
    return result;
  }

  void init(size_t capacity) {
    m_size        = 0;
    m_capacity    = capacity;
    m_buffer      = allocateBuffer(capacity);
    m_updateCount = 0;
  }

  int getChainLength(size_t index) const {
    int count = 0;
    for(CompactHashElement<K> *p = m_buffer[index]; p; p = p->m_next) {
      count++;
    }
    return count;
  }

public:
  CompactHashSet() {
    init(0);
  }

  explicit CompactHashSet(size_t capacity) {
    init(capacity);
  }

  CompactHashSet(const CompactHashSet<K> &src) {
    init(src.m_capacity);
    addAll(src);
  }

  CompactHashSet<K> &operator=(const CompactHashSet<K> &src) {
    if(this == &src) {
      return *this;
    }
    clear();
    setCapacity(src.size());
    addAll(src);
    return *this;
  }

  ~CompactHashSet() {
    clear();
  }

  inline bool hasOrder() const {
    return false;
  }

  void setCapacity(size_t capacity) {
    if(capacity < m_size) {
      capacity = m_size;
    }
    if(capacity == m_capacity) {
      return;
    }
    CompactHashElement<K> **oldBuffer   = m_buffer;
    const size_t            oldCapacity = m_capacity;

    m_capacity = capacity;
    m_buffer   = allocateBuffer(capacity);

    if(!isEmpty()) {
      for(size_t i = 0; i < oldCapacity; i++) {
        CompactHashElement<K> *n = oldBuffer[i];
        while(n) {
          const ULONG index = n->m_key.hashCode() % m_capacity;
          CompactHashElement<K> *&bp = m_buffer[index];
          CompactHashElement<K> *next = n->m_next;
          n->m_next = bp;
          bp        = n;
          n         = next;
        }
      }
    }
    if(oldBuffer) {
      delete[] oldBuffer;
    }
  }

  inline size_t getCapacity() const {
    return m_capacity;
  }

  inline int getPageCount() const {
    return m_elementPool.getPageCount();
  }

  bool add(const K &key) {
    ULONG index;
    if(m_capacity) {
      index = key.hashCode() % m_capacity;
      for(CompactHashElement<K> *p = m_buffer[index]; p; p = p->m_next) {
        if(key == p->m_key) {
          return false;
        }
      }
    }
    if(m_size+1 > m_capacity*3) {
      setCapacity(m_size*5+5);
      index = key.hashCode() % m_capacity; // no need to search key again. if m_capacity was 0, the set is empty
    }
    CompactHashElement<K> *p = m_elementPool.fetchElement();
    p->m_key                 = key;
    p->m_next                = m_buffer[index];
    m_buffer[index]          = p;
    m_size++;
    m_updateCount++;
    return true;
  }

  bool remove(const K &key) {
    if(m_capacity) {
      const ULONG index = key.hashCode() % m_capacity;
      for(CompactHashElement<K> *p = m_buffer[index], *last = NULL; p; last = p, p = p->m_next) {
        if(key == p->m_key) {
          if(last) {
            last->m_next = p->m_next;
          } else {
            m_buffer[index] = p->m_next;
          }
          m_elementPool.releaseElement(p);
          m_size--;
          m_updateCount++;
          return true;
        }
      }
    }
    return false;
  }

  bool contains(const K &key) const {
    if(m_capacity) {
      const ULONG index = key.hashCode() % m_capacity;
      for(const CompactHashElement<K> *p = m_buffer[index]; p; p = p->m_next) {
        if(key == p->m_key) {
          return true;
        }
      }
    }
    return false;
  }

  void clear() {
    m_elementPool.releaseAll();
    if(m_size) {
      m_size = 0;
      m_updateCount++;
    }
    setCapacity(0);
  }

  inline size_t size() const {
    return m_size;
  }

  inline bool isEmpty() const {
    return m_size == 0;
  }

  CompactIntArray getLength() const {
    const size_t capacity = getCapacity();
    CompactIntArray tmp(capacity);
    int m = 0;
    for(size_t index = 0; index < capacity; index++) {
      const int l = getChainLength(index);
      tmp.add(l);
      if(l > m) {
        m = l;
      }
    }
    CompactIntArray result(m+1);
    for(int i = 0; i <= m; i++) {
      result.add(0);
    }
    for(size_t index = 0; index < capacity; index++) {
      result[tmp[index]]++;
    }
    return result;
  }

  int getMaxChainLength() const {
    int m = 0;
    const size_t capacity = getCapacity();
    for(size_t i = 0; i < capacity; i++) {
      const int l = getChainLength(i);
      if(l > m) {
        m = l;
      }
    }
    return m;
  }

  // Adds every element in src to this. Return true if any elements were added.
  bool addAll(const CompactHashSet<K> &src) {
    bool changed = false;
    for(Iterator<K> it = src.getIterator(); it.hasNext(); ) {
      if(add(it.next())) {
        changed = true;
      }
    }
    if(changed) {
      m_updateCount++;
    }
    return changed;
  }

  bool addAll(const CompactArray<K> &a) {
    bool changed = false;
    for(size_t i = 0; i < a.size(); i++) {
      if(add(a[i])) changed = true;
    }
    return changed;
  }

  // Remove every element in set from this. Return true if any elements were removed.
  bool removeAll(const CompactHashSet<K> &set) {
    if(this == &set) {
      if(isEmpty()) return false;
      clear();
      return true;
    }
    bool changed = false;
    for(Iterator<K> it = set.getIterator(); it.hasNext();) {
      if(remove(it.next())) {
        changed = true;
      }
    }
    return changed;
  }

  // Remove every element from this that is not contained in set. Return true if any elements were removed.
  bool retainAll(const CompactHashSet<K> &set) {
    if(this == &set) return false; // Don't change anything. every element in this is in c too => nothing needs to be removed
    bool changed = false;
    for(Iterator<K> it = getIterator(); it.hasNext();) {
      if(!set.contains(it.next())) {
        it.remove();
        changed = true;
      }
    }
    return changed;
  }

  class CompactSetIterator : public AbstractIterator {
  private:
    CompactHashSet<K>     &m_set;
    CompactHashElement<K> *m_current, *m_next;
    size_t                 m_currentIndex;
    UINT64                 m_updateCount;

    void first() {
      m_current = NULL;
      for(m_currentIndex = 0; m_currentIndex < m_set.m_capacity; m_currentIndex++) {
        if(m_next = m_set.m_buffer[m_currentIndex]) {
          return;
        }
      }
      m_next = NULL;
    }

    inline void checkUpdateCount() const {
      if(m_updateCount != m_set.m_updateCount) {
        concurrentModificationError(_T("CompactSetIterator"));
      }
    }

  public:
    CompactSetIterator(CompactHashSet<K> *set) : m_set(*set), m_updateCount(set->m_updateCount) {
      first();
    }

    AbstractIterator *clone() {
      return new CompactSetIterator(*this);
    }

    bool hasNext() const {
      return m_next != NULL;
    }

    void *next() {
      if(m_next == NULL) {
        noNextElementError(_T("CompactSetIterator"));
      }
      checkUpdateCount();
      m_current = m_next;
      if((m_next = m_next->m_next) == NULL) {
        CompactHashElement<K> **first = m_set.m_buffer, **end = first + m_set.getCapacity();
        for(CompactHashElement<K> **p = first + m_currentIndex; ++p < end;) {
          if(*p) {
            m_next         = *p;
            m_currentIndex = p - first;
            break;
          }
        }
      }
      return &(m_current->m_key);
    }

    void remove() {
      if(m_current == NULL) {
        noCurrentElementError(_T("CompactSetIterator"));
      }
      checkUpdateCount();
      m_set.remove(m_current->m_key);
      m_current     = NULL;
      m_updateCount = m_set.m_updateCount;
    }
  };

  Iterator<K> getIterator() const {
    return Iterator<K>(new CompactSetIterator((CompactHashSet<K>*)this));
  }

  // Set intersection = set of elements that are in both sets.
  CompactHashSet<K> operator*(const CompactHashSet<K> &set) const {
    CompactHashSet<K> result;
    if(size() < set.size()) {
      for(Iterator<K> it = getIterator(); it.hasNext();) {
        const K &e = it.next();
        if(set.contains(e)) {
          result.add(e);
        }
      }
    } else {
      for(Iterator<K> it = set.getIterator(); it.hasNext();) {
        const K &e = it.next();
        if(contains(e)) {
          result.add(e);
        }
      }
    }
    return result;
  }

  // Set union = set of elements, that are in at least 1 of the sets.
  CompactHashSet<K> operator+(const CompactHashSet<K> &set) const {
    CompactHashSet<K> result(*this);
    result.addAll(set);
    return result;
  }

  // Set difference = set of elements in *this and not in set.
  CompactHashSet<K> operator-(const CompactHashSet<K> &set) const {
    CompactHashSet<K> result(*this);
    result.removeAll(set);
    return result;
  }

  // s1^s2 = (s1-s2) + (s2-s1) (symmetric difference) = set of elements that are in only one of the sets
  CompactHashSet<K> operator^(const CompactHashSet<K> &set) const {
    CompactHashSet<K> result;
    for(Iterator<K> it = getIterator(); it.hasNext();) {
      const K &e = it.next();
      if(!set.contains(e)) {
        result.add(e);
      }
    }
    for(Iterator<K> it = set.getIterator(); it.hasNext();) {
      const K &e = it.next();
      if(!contains(e)) {
        result.add(e);
      }
    }
    return result;
  }

  bool operator==(const CompactHashSet<K> &set) const {
    if(this == &set) return true;
    if (set.size() != size()) {
      return false;
    }
    for (Iterator<K> it = getIterator(); it.hasNext();) {
      if (!set.contains(it.next())) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const CompactHashSet<K> &set) const {
    return !(*this == set);
  }

  // Subset. Return true if all elements in *this are in set
  bool operator<=(const CompactHashSet<K> &set) const {
    for(Iterator<K> it = getIterator(); it.hasNext();) {
      if(!set.contains(it.next())) {
        return false;
      }
    }
    return true;
  }

  // Pure subset. return true if (*this <= set) && (size() < set.size())
  bool operator<(const CompactHashSet<K> &set) const {
    return (size() < set.size()) && (*this <= set);
  }

  bool operator>=(const CompactHashSet<K> &set) const {
    return set <= *this;
  }

  bool operator>(const CompactHashSet<K> &set) const {
    return set < *this;
  }

  void save(ByteOutputStream &s) const {
    const UINT  kSize = sizeof(K);
    const UINT64 count = size();

    s.putBytes((BYTE*)&kSize, sizeof(kSize));
    s.putBytes((BYTE*)&count, sizeof(count));
    CompactArray<K> a(1000);
    UINT64 wCount = 0;
    for(Iterator<K> it = getIterator(); it.hasNext();) {
      a.add(it.next());
      if(a.size() == 1000) {
        s.putBytes((BYTE*)a.getBuffer(),sizeof(K)*a.size());
        wCount += a.size();
        a.clear(-1);
      }
    }
    if(a.size() > 0) {
      s.putBytes((BYTE*)a.getBuffer(),sizeof(K)*a.size());
      wCount += a.size();
    }
    if (wCount != count) {
      throwException(_T("%s:#written elements:%I64u. setSize:%I64u")
                    ,__TFUNCTION__, wCount, count);
    }
  }

  void load(ByteInputStream &s) {
    UINT kSize;
    s.getBytesForced((BYTE*)&kSize, sizeof(kSize));
    if(kSize != sizeof(K)) {
      throwException(_T("sizeof(Key):%u. Size from stream=%u"), sizeof(K), kSize);
    }
    UINT64 size64;
    s.getBytesForced((BYTE*)&size64,sizeof(size64));
    CHECKUINT64ISVALIDSIZET(size64);
    size_t count = (size_t)size64;
    clear();
    setCapacity(count);
    for(size_t i = 0; i < count;) {
      K buffer[1000];
      const size_t n = min(count-i,ARRAYSIZE(buffer));
      s.getBytesForced((BYTE*)buffer, sizeof(buffer[0])*n);
      for(const K *p = buffer, *end = buffer + n; p < end;) {
        add(*(p++));
      }
      i += n;
    }
  }
};

typedef CompactHashSet<CompactShortKeyType > CompactShortHashSet;
typedef CompactHashSet<CompactUShortKeyType> CompactUShortHashSet;
typedef CompactHashSet<CompactIntKeyType   > CompactIntHashSet;
typedef CompactHashSet<CompactUIntKeyType  > CompactUIntHashSet;
typedef CompactHashSet<CompactFloatKeyType > CompactFloatHashSet;
typedef CompactHashSet<CompactDoubleKeyType> CompactDoubleHashSet;
