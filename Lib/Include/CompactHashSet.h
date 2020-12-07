#pragma once

#include "Set.h"
#include "CompactKeyType.h"
#include "HeapObjectPool.h"

template<typename K> class SetEntry {
public:
  K m_key;
  inline SetEntry() {}
  inline SetEntry(const K &key) : m_key(key) {
  }
};

// Assume K has public member-function ULONG hashCode() const...
// and bool operator==(const K &) defined
template<typename K, UINT pageSize=20000> class CompactHashSet : public CollectionBase<K> {
private:
  size_t                                             m_size;
  size_t                                             m_capacity;
  LinkObject<SetEntry<K> >                         **m_buffer;
  HeapObjectPool<LinkObject<SetEntry<K> >, pageSize> m_entryPool;
  UINT64                                             m_updateCount;

  LinkObject<SetEntry<K> > **allocateBuffer(size_t capacity) const {
    LinkObject<SetEntry<K> > **result = capacity ? new LinkObject<SetEntry<K> >*[capacity] : nullptr; TRACE_NEW(result);
    return resetPointerArray(result, capacity);
  }

  void init(size_t capacity) {
    m_size        = 0;
    m_capacity    = capacity;
    m_buffer      = allocateBuffer(capacity);
    m_updateCount = 0;
  }

  LinkObject<SetEntry<K> > *selectNonNullLinkObject(RandomGenerator &rnd) const {
    if(size() == 0) throwEmptySetException(__TFUNCTION__);
    const size_t n = m_capacity, k = randSizet(n, rnd);
    LinkObject<SetEntry<K> > **first = m_buffer, **endp = first + n, **lpk = first + k, **p = lpk;
    for(; (p < endp) && (*p == nullptr); p++);
    if(p == endp) {
      for(p = lpk - 1; (p >= first) && (*p == nullptr); p--);
    }
    return *p;
  }

  LinkObject<SetEntry<K> > *selectLinkObject(RandomGenerator &rnd) const {
    for(auto p = selectNonNullLinkObject(rnd);; p = p->m_next) {
      if((p->m_next == nullptr) || rnd.nextBool()) {
        return p;
      }
    }
    return nullptr;
  }

  UINT getChainLength(size_t index) const {
    UINT count = 0;
    for(auto p = m_buffer[index]; p; p = p->m_next) {
      count++;
    }
    return count;
  }

public:
  inline CompactHashSet() {
    init(0);
  }

  explicit CompactHashSet(size_t capacity) {
    init(capacity);
  }

  CompactHashSet(const CompactHashSet &src) {
    init(src.m_capacity);
    addAll(src);
  }

  CompactHashSet &operator=(const CompactHashSet &src) {
    if(this != &src) {
      clear(src.getCapacity()).addAll(src);
    }
    return *this;
  }

  ~CompactHashSet() override {
    clear();
  }

  CompactHashSet &setCapacity(size_t capacity) {
    if(capacity < m_size) {
      capacity = m_size;
    }
    if(capacity == m_capacity) {
      return *this;
    }
    LinkObject<SetEntry<K> > **oldBuffer   = m_buffer;
    const size_t               oldCapacity = m_capacity;

    m_capacity = capacity;
    m_buffer   = allocateBuffer(capacity);

    if(!isEmpty()) {
      for(size_t i = 0; i < oldCapacity; i++) {
        for(auto n = oldBuffer[i]; n;) {
          const ULONG index = n->m_e.m_key.hashCode() % m_capacity;
          LinkObject<SetEntry<K> > *&bp = m_buffer[index];
          LinkObject<SetEntry<K> > *next = n->m_next;
          n->m_next = bp;
          bp        = n;
          n         = next;
        }
      }
    }
    SAFEDELETEARRAY(oldBuffer);
    return *this;
  }

  inline size_t getCapacity() const {
    return m_capacity;
  }

  // If capacity < 0, it's left unchanged
  // Return *this
  CompactHashSet &clear(intptr_t capacity) {
    m_entryPool.releaseAll();
    if(m_size) {
      m_size = 0;
      m_updateCount++;
    }
    if(capacity >= 0) {
      setCapacity(capacity);
    }
    resetPointerArray(m_buffer, getCapacity());
    return *this;
  }

  void clear() override {
    clear(0);
  }

  size_t size() const override {
    return m_size;
  }

  bool add(const K &key) override {
    ULONG index;
    if(m_capacity) {
      index = key.hashCode() % m_capacity;
      for(auto p = m_buffer[index]; p; p = p->m_next) {
        if(key == p->m_e.m_key) {
          return false;
        }
      }
    }
    if(m_size+1 > m_capacity*3) {
      setCapacity(m_size*5+5);
      index = key.hashCode() % m_capacity; // no need to search key again. if m_capacity was 0, the set is empty
    }
    LinkObject<SetEntry<K> > *p = m_entryPool.fetch();
    p->m_e.m_key                = key;
    p->m_next                   = m_buffer[index];
    m_buffer[index]             = p;
    m_size++;
    m_updateCount++;
    return true;
  }

  bool remove(const K &key) {
    if(m_capacity) {
      const ULONG index = key.hashCode() % m_capacity;
      LinkObject<SetEntry<K> > *last = nullptr;
      for(auto p = m_buffer[index]; p; last = p, p = p->m_next) {
        if(key == p->m_e.m_key) {
          if(last) {
            last->m_next = p->m_next;
          } else {
            m_buffer[index] = p->m_next;
          }
          m_entryPool.release(p);
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
      for(auto p = m_buffer[index]; p; p = p->m_next) {
        if(key == p->m_e.m_key) {
          return true;
        }
      }
    }
    return false;
  }

  const K &select(RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) const {
    return selectLinkObject(rnd)->m_e.m_key;
  }

  K &select(RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
    return selectLinkObject(rnd)->m_e.m_key;
  }

  inline UINT getPageCount() const {
    return m_entryPool.getPageCount();
  }

  CompactUIntArray getLength() const {
    const size_t capacity = getCapacity();
    CompactUIntArray tmp(capacity);
    UINT m = 0;
    for(size_t index = 0; index < capacity; index++) {
      const UINT l = getChainLength(index);
      tmp.add(l);
      if(l > m) {
        m = l;
      }
    }
    CompactUIntArray result(m+1);
    result.insert(0, (UINT)0, m + 1);
    for(size_t index = 0; index < capacity; index++) {
      result[tmp[index]]++;
    }
    return result;
  }

  UINT getMaxChainLength() const {
    UINT m = 0;
    const size_t capacity = getCapacity();
    for(size_t i = 0; i < capacity; i++) {
      const UINT l = getChainLength(i);
      if(l > m) {
        m = l;
      }
    }
    return m;
  }

  // Add every element in set to this. Return true if any elements were added.
  bool addAll(const CollectionBase<K> &c) override {
    if(this == &c) {
      return false;
    }
    return __super::addAll(c);
  }
  bool removeAll(const ConstIterator<K> &it) {
    const size_t oldSize = size();
    for(auto it1 = it; it1.hasNext(); ) {
      remove(it1.next());
    }
    return size() != oldSize;
  }
  // Remove every element in set from this. Return true if any elements were removed.
  bool removeAll(const CollectionBase<K> &c) {
    if(this == &c) {
      if(isEmpty()) return false;
      clear();
      return true;
    }
    return removeAll(c.getIterator());
  }

  // Remove every element from this that is not contained in set. Return true if any elements were removed.
  bool retainAll(const CompactHashSet &set) {
    if(this == &set) {
      return false; // Don't change anything. every element in this is in set too => nothing needs to be removed
    }
    const size_t n = size();
    for(auto it = getIterator(); it.hasNext();) {
      if(!set.contains(it.next())) {
        it.remove();
      }
    }
    return size() != n;
  }

  // Assume m_set.m_buffer doesn't change while iterator exist
  // as there are pointers directy into the sets buffer-array
  class CompactSetIterator : public AbstractIterator {
  private:
    CompactHashSet           &m_set;
    LinkObject<SetEntry<K> > *m_current, *m_next, **m_bufp, **m_endBuf;
    UINT64                    m_updateCount;

    void first() {
      m_current = nullptr;
      if(m_set.m_buffer) {
        m_endBuf = m_set.m_buffer + m_set.getCapacity();
        for(auto p = m_set.m_buffer; p < m_endBuf; p++) {
          if(*p) {
            m_bufp = p;
            m_next = *p;
            return;
          }
        }
      }
      m_endBuf = m_bufp = nullptr;
      m_next   = nullptr;
    }

    inline void checkUpdateCount(const TCHAR *method) const {
      if(m_updateCount != m_set.m_updateCount) {
        concurrentModificationError(method);
      }
    }

  public:
    CompactSetIterator(const CompactHashSet *set) : m_set(*(CompactHashSet*)set), m_updateCount(set->m_updateCount) {
      first();
    }
    AbstractIterator *clone()            override {
      return new CompactSetIterator(*this);
    }
    bool              hasNext()    const override {
      return m_next != nullptr;
    }
    void             *next()             override {
      if(m_next == nullptr) {
        noNextElementError(__TFUNCTION__);
      }
      __assume(m_next);
      checkUpdateCount(__TFUNCTION__);
      m_current = m_next;
      if((m_next = m_next->m_next) == nullptr) {
        for(auto p = m_bufp; ++p < m_endBuf;) {
          if(*p) {
            m_bufp = p;
            m_next = *p;
            break;
          }
        }
      }
      return &(m_current->m_e.m_key);
    }
    void remove()                        override {
      if(m_current == nullptr) {
        noCurrentElementError(__TFUNCTION__);
      }
      __assume(m_current);
      checkUpdateCount(__TFUNCTION__);
      m_set.remove(m_current->m_e.m_key);
      m_current     = nullptr;
      m_updateCount = m_set.m_updateCount;
    }
  };

  ConstIterator<K> getIterator() const override {
    return ConstIterator<K>(new CompactSetIterator(this));
  }
  Iterator<K>      getIterator()       override {
    return Iterator<K>(new CompactSetIterator(this));
  }

  bool hasOrder() const override {
    return false;
  }

  // Set intersection = set of elements that are in both sets.
  CompactHashSet operator*(const CompactHashSet &set) const {
    CompactHashSet result;
    if(size() < set.size()) {
      for(auto it = getIterator(); it.hasNext();) {
        const K &e = it.next();
        if(set.contains(e)) {
          result.add(e);
        }
      }
    } else {
      for(auto it = set.getIterator(); it.hasNext();) {
        const K &e = it.next();
        if(contains(e)) {
          result.add(e);
        }
      }
    }
    return result;
  }

  // Set union = set of elements, that are in at least 1 of the sets.
  CompactHashSet operator+(const CompactHashSet &set) const {
    CompactHashSet result(*this);
    result.addAll(set);
    return result;
  }

  // Set difference = set of elements in *this and not in set.
  CompactHashSet operator-(const CompactHashSet &set) const {
    CompactHashSet result(*this);
    result.removeAll(set);
    return result;
  }

  // s1^s2 = (s1-s2) + (s2-s1) (symmetric difference) = set of elements that are in only one of the sets
  CompactHashSet operator^(const CompactHashSet &set) const {
    CompactHashSet result;
    for(auto it = getIterator(); it.hasNext();) {
      const K &e = it.next();
      if(!set.contains(e)) {
        result.add(e);
      }
    }
    for(auto it = set.getIterator(); it.hasNext();) {
      const K &e = it.next();
      if(!contains(e)) {
        result.add(e);
      }
    }
    return result;
  }

  bool operator==(const CompactHashSet &set) const {
    if(this == &set) return true;
    if(set.size() != size()) {
      return false;
    }
    for(auto it = getIterator(); it.hasNext();) {
      if(!set.contains(it.next())) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const CompactHashSet &set) const {
    return !(*this == set);
  }

  // Subset. Return true if all elements in *this are in set
  bool operator<=(const CompactHashSet &set) const {
    if(size() > set.size()) return false;
    for(auto it = getIterator(); it.hasNext();) {
      if(!set.contains(it.next())) {
        return false;
      }
    }
    return true;
  }

  // Pure subset. return true if(*this <= set) && (size() < set.size())
  inline bool operator<(const CompactHashSet &set) const {
    return (size() < set.size()) && (*this <= set);
  }

  inline bool operator>=(const CompactHashSet &set) const {
    return set <= *this;
  }

  inline bool operator>(const CompactHashSet &set) const {
    return set < *this;
  }
};

#define DEFINECOMPACTHASHSETTEMPLATE(ktype)                                                                                             \
template<UINT pageSize=20000> class Compact##ktype##HashSet : public CompactHashSet<Compact##ktype##KeyType, pageSize> {                \
  public:                                                                                                                               \
  Compact##ktype##HashSet() {                                                                                                           \
  }                                                                                                                                     \
  explicit Compact##ktype##HashSet(size_t capacity) : CompactHashSet(capacity) {                                                        \
  }                                                                                                                                     \
  Compact##ktype##HashSet(const CompactHashSet &src) : CompactHashSet(src) {                                                            \
  }                                                                                                                                     \
  Compact##ktype##HashSet &operator=(const CompactHashSet &src) {                                                                       \
    __super::operator=(src);                                                                                                            \
    return *this;                                                                                                                       \
  }                                                                                                                                     \
}

DEFINECOMPACTHASHSETTEMPLATE(Short );
DEFINECOMPACTHASHSETTEMPLATE(UShort);
DEFINECOMPACTHASHSETTEMPLATE(Int   );
DEFINECOMPACTHASHSETTEMPLATE(UInt  );
DEFINECOMPACTHASHSETTEMPLATE(Str   );
DEFINECOMPACTHASHSETTEMPLATE(StrI  );
