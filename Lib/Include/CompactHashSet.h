#pragma once

#include "Set.h"
#include "CompactKeyType.h"
#include "HeapObjectPool.h"

template <typename K> class SetEntry {
public:
  K m_key;
  inline SetEntry() {}
  inline SetEntry(const K &key) : m_key(key) {
  }
};

// Assume K has public member-function ULONG hashCode() const...
// and bool operator==(const K &) defined
template <typename K> class CompactHashSet {
private:
  size_t                                    m_size;
  size_t                                    m_capacity;
  LinkObject<SetEntry<K> >                **m_buffer;
  HeapObjectPool<LinkObject<SetEntry<K> > > m_entryPool;
  UINT64                                    m_updateCount;

  LinkObject<SetEntry<K> > **allocateBuffer(size_t capacity) const {
    LinkObject<SetEntry<K> > **result = capacity ? new LinkObject<SetEntry<K> >*[capacity] : NULL; TRACE_NEW(result);
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
    for(LinkObject<SetEntry<K> > *p = m_buffer[index]; p; p = p->m_next) {
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
    if(this == &src) {
      return *this;
    }
    clear();
    setCapacity(src.size());
    addAll(src);
    return *this;
  }

  virtual ~CompactHashSet() {
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
    LinkObject<SetEntry<K> > **oldBuffer   = m_buffer;
    const size_t               oldCapacity = m_capacity;

    m_capacity = capacity;
    m_buffer   = allocateBuffer(capacity);

    if(!isEmpty()) {
      for(size_t i = 0; i < oldCapacity; i++) {
        for(LinkObject<SetEntry<K> > *n = oldBuffer[i]; n;) {
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
  }

  inline size_t getCapacity() const {
    return m_capacity;
  }

  inline int getPageCount() const {
    return m_entryPool.getPageCount();
  }

  bool add(const K &key) {
    ULONG index;
    if(m_capacity) {
      index = key.hashCode() % m_capacity;
      for(LinkObject<SetEntry<K> > *p = m_buffer[index]; p; p = p->m_next) {
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
      for(LinkObject<SetEntry<K> > *p = m_buffer[index], *last = NULL; p; last = p, p = p->m_next) {
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
      for(const LinkObject<SetEntry<K> > *p = m_buffer[index]; p; p = p->m_next) {
        if(key == p->m_e.m_key) {
          return true;
        }
      }
    }
    return false;
  }

  void clear() {
    m_entryPool.releaseAll();
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
  bool addAll(const CompactHashSet &set) {
    if(this == &set) {
      return false;
    }
    const size_t n = size();
    for(Iterator<K> it = set.getIterator(); it.hasNext(); ) {
      add(it.next());
    }
    return size() != n;
  }

  // Remove every element in set from this. Return true if any elements were removed.
  bool removeAll(const CompactHashSet &set) {
    if(this == &set) {
      if(isEmpty()) return false;
      clear();
      return true;
    }
    const size_t n = size();
    for(Iterator<K> it = set.getIterator(); it.hasNext();) {
      remove(it.next());
    }
    return size() != n;
  }

  bool addAll(const CompactArray<K> &a) {
    const size_t n = size();
    for(K e : a) {
      add(e);
    }
    return size() != n;
  }

  bool removeAll(const CompactArray<K> &a) {
    const size_t n = size();
    for(K e : a) {
      remove(e);
    }
    return size() != n;
  }

  // Remove every element from this that is not contained in set. Return true if any elements were removed.
  bool retainAll(const CompactHashSet &set) {
    if(this == &set) {
      return false; // Don't change anything. every element in this is in set too => nothing needs to be removed
    }
    const size_t n = size();
    for(Iterator<K> it = getIterator(); it.hasNext();) {
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
      m_current = NULL;
      if(m_set.m_buffer) {
        m_endBuf = m_set.m_buffer + m_set.getCapacity();
        for(LinkObject<SetEntry<K> > **p = m_set.m_buffer; p < m_endBuf; p++) {
          if(*p) {
            m_bufp = p;
            m_next = *p;
            return;
          }
        }
      }
      m_endBuf = m_bufp = NULL;
      m_next   = NULL;
    }

    inline void checkUpdateCount() const {
      if(m_updateCount != m_set.m_updateCount) {
        concurrentModificationError(_T("CompactSetIterator"));
      }
    }

  public:
    CompactSetIterator(CompactHashSet *set) : m_set(*set), m_updateCount(set->m_updateCount) {
      first();
    }
    AbstractIterator *clone()       override {
      return new CompactSetIterator(*this);
    }
    bool hasNext()            const override {
      return m_next != NULL;
    }
    void *next()                    override {
      if(m_next == NULL) {
        noNextElementError(_T("CompactSetIterator"));
      }
      __assume(m_next);
      checkUpdateCount();
      m_current = m_next;
      if((m_next = m_next->m_next) == NULL) {
        for(LinkObject<SetEntry<K> > **p = m_bufp; ++p < m_endBuf;) {
          if(*p) {
            m_bufp = p;
            m_next = *p;
            break;
          }
        }
      }
      return &(m_current->m_e.m_key);
    }
    void remove()                   override {
      if(m_current == NULL) {
        noCurrentElementError(_T("CompactSetIterator"));
      }
      __assume(m_current);
      checkUpdateCount();
      m_set.remove(m_current->m_e.m_key);
      m_current     = NULL;
      m_updateCount = m_set.m_updateCount;
    }
  };

  Iterator<K> getIterator() const {
    return Iterator<K>(new CompactSetIterator((CompactHashSet*)this));
  }

  // Set intersection = set of elements that are in both sets.
  CompactHashSet operator*(const CompactHashSet &set) const {
    CompactHashSet result;
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

  bool operator==(const CompactHashSet &set) const {
    if(this == &set) return true;
    if(set.size() != size()) {
      return false;
    }
    for(Iterator<K> it = getIterator(); it.hasNext();) {
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
    for(Iterator<K> it = getIterator(); it.hasNext();) {
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
    if(wCount != count) {
      throwException(_T("%s:#written elements:%I64u. setSize:%I64u")
                    ,__TFUNCTION__, wCount, count);
    }
  }

  void load(ByteInputStream &s) {
    UINT kSize;
    s.getBytesForced((BYTE*)&kSize, sizeof(kSize));
    if(kSize != sizeof(K)) {
      throwException(_T("sizeof(Key):%zu. Size from stream=%u"), sizeof(K), kSize);
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
