#pragma once

#include <Map.h>
#include "CompactHashSet.h"

template <class K, class V> class MapEntry : public SetEntry<K> {
public:
  V m_value;
  inline MapEntry() {
  }
  inline MapEntry(const K &key, const V &value) : SetEntry(key), m_value(value) {
  }
};

template <class K, class V> class CompactHashMap {
private:
  size_t                                       m_size;
  size_t                                       m_capacity;
  LinkObject<MapEntry<K,V> >                 **m_buffer;
  HeapObjectPool<LinkObject<MapEntry<K,V> > >  m_entryPool;
  UINT64                                       m_updateCount;

  LinkObject<MapEntry<K,V> > **allocateBuffer(size_t capacity) const {
    LinkObject<MapEntry<K,V> > **result = capacity ? new LinkObject<MapEntry<K,V> >*[capacity] : NULL; TRACE_NEW(result);
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
    for(LinkObject<MapEntry<K,V> > *p = m_buffer[index]; p; p = p->m_next) {
      count++;
    }
    return count;
  }

public:
  CompactHashMap() {
    init(0);
  }

  explicit CompactHashMap(size_t capacity) {
    init(capacity);
  }

  CompactHashMap(const CompactHashMap &src) {
    init(src.m_capacity);
    addAll(src);
  }

  CompactHashMap &operator=(const CompactHashMap &src) {
    if(this == &src) {
      return *this;
    }
    clear();
    setCapacity(src.size());
    addAll(src);
    return *this;
  }

  virtual ~CompactHashMap() {
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
    LinkObject<MapEntry<K,V> > **oldBuffer   = m_buffer;
    const size_t                 oldCapacity = m_capacity;

    m_capacity = capacity;
    m_buffer   = allocateBuffer(capacity);

    if(!isEmpty()) {
      for(size_t i = 0; i < oldCapacity; i++) {
        for(LinkObject<MapEntry<K,V> > *n = oldBuffer[i]; n;) {
          const ULONG index = n->m_e.m_key.hashCode() % m_capacity;
          LinkObject<MapEntry<K,V> > *&bp  = m_buffer[index];
          LinkObject<MapEntry<K,V> > *next = n->m_next;
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

  bool put(const K &key, const V &value) {
    ULONG index;
    if(m_capacity) {
      index = key.hashCode() % m_capacity;
      for(LinkObject<MapEntry<K,V> > *p = m_buffer[index]; p; p = p->m_next) {
        if(key == p->m_e.m_key) {
          return false;
        }
      }
    }
    if(m_size+1 > m_capacity*3) {
      setCapacity(m_size*5+5);
      index = key.hashCode() % m_capacity; // no need to search key again. if m_capacity was 0, the set is empty
    }
    LinkObject<MapEntry<K,V> > *p = m_entryPool.fetch();
    p->m_e.m_key                  = key;
    p->m_e.m_value                = value;
    p->m_next                     = m_buffer[index];
    m_buffer[index]               = p;
    m_size++;
    m_updateCount++;
    return true;
  }

  bool remove(const K &key) {
    if(m_capacity) {
      const ULONG index = key.hashCode() % m_capacity;
      for(LinkObject<MapEntry<K,V> > *p = m_buffer[index], *last = NULL; p; last = p, p = p->m_next) {
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

  V *get(const K &key) const {
    if(m_capacity) {
      const ULONG index = key.hashCode() % m_capacity;
      for(LinkObject<MapEntry<K,V> > *p = m_buffer[index]; p; p = p->m_next) {
        if(key == p->m_e.m_key) {
          return &(p->m_e.m_value);
        }
      }
    }
    return NULL;
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
  bool addAll(const CompactHashMap &map) {
    const size_t n = size();
    for(Iterator<Entry<K,V> > it = map.getEntryIterator(); it.hasNext(); ) {
      const Entry<K,V> &e = it.next();
      put(e.getKey(), e.getValue());
    }
    return size() != n;
  }

  // Remove every element in set from this. Return true if any elements were removed.
  bool removeAll(const CompactHashMap &map) {
    if(this == &map) {
      if(isEmpty()) return false;
      clear();
      return true;
    }
    const size_t n = size();
    for(Iterator<Entry<K,V> > it = map.getEntryIterator(); it.hasNext();) {
      remove(it.next().getKey());
    }
    return size() != n;
  }

  bool removeAll(const CompactArray<K> &a) {
    const size_t n = size();
    for(size_t i = 0; i < a.size(); i++) {
      remove(a[i]);
    }
    return size() != n;
  }

  class CompactMapIteratorEntry : public AbstractEntry {
  private:
     MapEntry<K,V> *m_entry;
  public:
    CompactMapIteratorEntry() {
      m_entry = NULL;
    }

    inline CompactMapIteratorEntry &operator=(MapEntry<K,V> &entry) {
      m_entry = &entry;
      return *this;
    }

    const void *key() const {
      return &m_entry->m_key;
    }

    void *value() {
      return &m_entry->m_value;
    }

    const void *value() const {
      return &m_entry->m_value;
    }
  };

  class CompactMapEntryIterator : public AbstractIterator {
  private:
    CompactHashMap             &m_map;
    LinkObject<MapEntry<K,V> > *m_current, *m_next, **m_bufp, **m_endBuf;
    UINT64                      m_updateCount;

    void first() {
      m_current = NULL;
      if(m_map.m_buffer) {
        m_endBuf = m_map.m_buffer + m_map.getCapacity();
        for(LinkObject<MapEntry<K,V> > **p = m_map.m_buffer; p < m_endBuf; p++) {
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
      if (m_updateCount != m_map.m_updateCount) {
        concurrentModificationError(_T("CompactMapEntryIterator"));
      }
    }

  protected:
    CompactMapIteratorEntry m_entry;

  public:
    CompactMapEntryIterator(CompactHashMap *map) : m_map(*map), m_updateCount(map->m_updateCount) {
      first();
    }

    AbstractIterator *clone() {
      return new CompactMapEntryIterator(*this);
    }

    bool hasNext() const {
      return m_next != NULL;
    }

    void *next() {
      if(m_next == NULL) {
        noNextElementError(_T("CompactMapEntryIterator"));
      }
      __assume(m_next);
      checkUpdateCount();
      m_current = m_next;
      if((m_next = m_next->m_next) == NULL) {
        for(LinkObject<MapEntry<K,V> > **p = m_bufp; ++p < m_endBuf;) {
          if(*p) {
            m_bufp = p;
            m_next = *p;
            break;
          }
        }
      }
      m_entry = m_current->m_e;
      return &m_entry;
    }

    void remove() {
      if(m_current == NULL) {
        noCurrentElementError(_T("CompactMapEntryIterator"));
      }
      checkUpdateCount();
      __assume(m_current);
      m_map.remove(m_current->m_e.m_key);
      m_current = NULL;
      m_updateCount = m_map.m_updateCount;
    }
  };

  Iterator<Entry<K,V> > getEntryIterator() const {
    return Iterator<Entry<K,V> >(new CompactMapEntryIterator((CompactHashMap*)this));
  }


  class CompactMapKeyIterator : public CompactMapEntryIterator {
  public:
    CompactMapKeyIterator(CompactHashMap *map) : CompactMapEntryIterator(map) {
    }

    AbstractIterator *clone() {
      return new CompactMapKeyIterator(*this);
    }

    void *next() {
      __super::next();
      return (void*)m_entry.key();
    }
  };

  Iterator<K> getKeyIterator() const {
    return Iterator<K>(new CompactMapKeyIterator((CompactHashMap*)this));
  }

  bool operator==(const CompactHashMap &map) const {
    if (this == &map) return true;
    if (map.size() != size()) {
      return false;
    }
    for(Iterator<Entry<K,V> > it = getEntryIterator(); it.hasNext();) {
      const Entry<K,V> &e = it.next();
      const V *v = map.get(e.getKey());
      if((v == NULL) || (*v != e.getValue())) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const CompactHashMap &map) const {
    return !(*this == map);
  }

  void save(ByteOutputStream &s) const {
    const UINT   kSize = sizeof(K);
    const UINT   vSize = sizeof(V);
    const UINT64 count = size();

    s.putBytes((BYTE*)&kSize, sizeof(kSize));
    s.putBytes((BYTE*)&vSize, sizeof(vSize));
    s.putBytes((BYTE*)&count, sizeof(count));
    CompactArray<MapEntry<K,V> > a(1000);
    UINT64 wCount = 0;
    for(Iterator<Entry<K,V> > it = getEntryIterator(); it.hasNext();) {
      const Entry<K,V> &e = it.next();
      a.add(MapEntry<K,V>(e.getKey(), e.getValue()));
      if(a.size() == 1000) {
        s.putBytes((BYTE*)a.getBuffer(),sizeof(MapEntry<K,V>)*a.size());
        wCount += a.size();
        a.clear(-1);
      }
    }
    if(a.size() > 0) {
      s.putBytes((BYTE*)a.getBuffer(),sizeof(MapEntry<K,V>)*a.size());
      wCount += a.size();
    }
    if (wCount != count) {
      throwException(_T("%s:#written elements:%I64u. setSize:%I64u")
                    ,__TFUNCTION__, wCount, count);
    }
  }

  void load(ByteInputStream &s) {
    UINT kSize,vSize;
    s.getBytesForced((BYTE*)&kSize, sizeof(kSize));
    s.getBytesForced((BYTE*)&vSize, sizeof(vSize));
    if(kSize != sizeof(K)) {
      throwException(_T("sizeof(Key):%zu. Size from stream=%u"), sizeof(K), kSize);
    }
    if(vSize != sizeof(V)) {
      throwException(_T("sizeof(Value):%zu. Size from stream=%u"), sizeof(V), vSize);
    }
    UINT64 size64;
    s.getBytesForced((BYTE*)&size64,sizeof(size64));
    CHECKUINT64ISVALIDSIZET(size64);
    size_t count = (size_t)size64;
    clear();
    setCapacity(count);
    for(size_t i = 0; i < count;) {
      MapEntry<K,V> buffer[1000];
      const size_t n = min(count-i,ARRAYSIZE(buffer));
      s.getBytesForced((BYTE*)buffer, sizeof(buffer[0])*n);
      for(const MapEntry<K,V> *p = buffer, *end = buffer + n; p < end; p++) {
        put(p->m_key, p->m_value);
      }
      i += n;
    }
  }
};

template <class T> class CompactShortHashMap : public CompactHashMap<CompactShortKeyType, T> {
public:
  CompactShortHashMap() {
  }
  explicit CompactShortHashMap(size_t capacity)  : CompactHashMap<CompactShortKeyType, T>(capacity) {
  }
  CompactShortHashMap(const CompactShortHashMap<T> &src) : CompactHashMap<CompactShortKeyType, T>(src) {
  }
};

template <class T> class CompactUShortHashMap : public CompactHashMap<CompactUShortKeyType, T> {
public:
  CompactUShortHashMap() {
  }
  explicit CompactUShortHashMap(size_t capacity)  : CompactHashMap<CompactUShortKeyType, T>(capacity) {
  }
  CompactUShortHashMap(const CompactUShortHashMap<T> &src) : CompactHashMap<CompactUShortKeyType, T>(src) {
  }
};

template <class T> class CompactIntHashMap : public CompactHashMap<CompactIntKeyType, T> {
public:
  CompactIntHashMap() {
  }
  explicit CompactIntHashMap(size_t capacity)  : CompactHashMap<CompactIntKeyType, T>(capacity) {
  }
  CompactIntHashMap(const CompactIntHashMap<T> &src) : CompactHashMap<CompactIntKeyType, T>(src) {
  }
};

template <class T> class CompactUIntHashMap : public CompactHashMap<CompactUIntKeyType, T> {
public:
  CompactUIntHashMap() {
  }
  explicit CompactUIntHashMap(size_t capacity)  : CompactHashMap<CompactUIntKeyType, T>(capacity) {
  }
  CompactUIntHashMap(const CompactUIntHashMap<T> &src) : CompactHashMap<CompactUIntKeyType, T>(src) {
  }
};

template <class T> class CompactFloatHashMap : public CompactHashMap<CompactFloatKeyType, T> {
public:
  CompactFloatHashMap() {
  }
  explicit CompactFloatHashMap(size_t capacity)  : CompactHashMap<CompactFloatKeyType, T>(capacity) {
  }
  CompactFloatHashMap(const CompactFloatHashMap<T> &src) : CompactHashMap<CompactFloatKeyType, T>(src) {
  }
};

template <class T> class CompactDoubleHashMap : public CompactHashMap<CompactDoubleKeyType, T> {
public:
  CompactDoubleHashMap() {
  }
  explicit CompactDoubleHashMap(size_t capacity)  : CompactHashMap<CompactDoubleKeyType, T>(capacity) {
  }
  CompactDoubleHashMap(const CompactDoubleHashMap<T> &src) : CompactHashMap<CompactDoubleKeyType, T>(src) {
  }
};

template <class T> class CompactStrHashMap    : public CompactHashMap<CompactStrKeyType, T> {
public:
  CompactStrHashMap() {
  }
  explicit CompactStrHashMap(size_t capacity)  : CompactHashMap<CompactStrKeyType, T>(capacity) {
  }
  CompactStrHashMap(const CompactStrHashMap<T> &src) : CompactHashMap<CompactStrKeyType, T>(src) {
  }
};

template <class T> class CompactStrIHashMap    : public CompactHashMap<CompactStrIKeyType, T> {
public:
  CompactStrIHashMap() {
  }
  explicit CompactStrIHashMap(size_t capacity)  : CompactHashMap<CompactStrIKeyType, T>(capacity) {
  }
  CompactStrIHashMap(const CompactStrIHashMap<T> &src) : CompactHashMap<CompactStrIKeyType, T>(src) {
  }
};
