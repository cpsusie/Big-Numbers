#pragma once

#include "Map.h"
#include "CompactHashSet.h"

template <typename K, typename V> class MapEntry : public SetEntry<K> {
public:
  V m_value;
  inline MapEntry() {
  }
  inline MapEntry(const K &key, const V &value) : SetEntry(key), m_value(value) {
  }
};

template <typename K, typename V, UINT pageSize=20000> class CompactHashMap : public MapBase<K,V> {
private:
  size_t                                                m_size;
  size_t                                                m_capacity;
  LinkObject<MapEntry<K,V> >                          **m_buffer;
  HeapObjectPool<LinkObject<MapEntry<K,V> >, pageSize>  m_entryPool;
  UINT64                                                m_updateCount;

  LinkObject<MapEntry<K,V> > **allocateBuffer(size_t capacity) const {
    LinkObject<MapEntry<K,V> > **result = capacity ? new LinkObject<MapEntry<K,V> >*[capacity] : nullptr; TRACE_NEW(result);
    return resetPointerArray(result, capacity);
  }

  void init(size_t capacity) {
    m_size        = 0;
    m_capacity    = capacity;
    m_buffer      = allocateBuffer(capacity);
    m_updateCount = 0;
  }

  int getChainLength(size_t index) const {
    int count = 0;
    for(auto p = m_buffer[index]; p; p = p->m_next) {
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
    if(this != &src) {
      clear(src.getCapacity()).addAll(src);
    }
    return *this;
  }

  ~CompactHashMap() override {
    clear();
  }

  // If capacity < 0, it's left unchanged
  // Return *this
  CompactHashMap &clear(intptr_t capacity) {
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

  bool put(const K &key, const V &value) override {
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
    LinkObject<MapEntry<K,V> > *p = m_entryPool.fetch();
    p->m_e.m_key                  = key;
    p->m_e.m_value                = value;
    p->m_next                     = m_buffer[index];
    m_buffer[index]               = p;
    m_size++;
    m_updateCount++;
    return true;
  }

private:
  inline const V *getValue(const K &key) const {
    if(m_capacity) {
      const ULONG index = key.hashCode() % m_capacity;
      for(auto p = m_buffer[index]; p; p = p->m_next) {
        if(key == p->m_e.m_key) {
          return &(p->m_e.m_value);
        }
      }
    }
    return nullptr;
  }

public:
  const V *get(const K &key) const override {
    return getValue(key);
  }
  V *get(const K &key) override {
    return (V*)getValue(key);
  }

  bool remove(const K &key) override {
    if(m_capacity) {
      const ULONG index = key.hashCode() % m_capacity;
      LinkObject<MapEntry<K, V> > *last = nullptr;
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

  CompactHashMap &setCapacity(size_t capacity) {
    if(capacity < m_size) {
      capacity = m_size;
    }
    if(capacity == m_capacity) {
      return *this;
    }
    LinkObject<MapEntry<K,V> > **oldBuffer   = m_buffer;
    const size_t                 oldCapacity = m_capacity;

    m_capacity = capacity;
    m_buffer   = allocateBuffer(capacity);

    if(!isEmpty()) {
      for(size_t i = 0; i < oldCapacity; i++) {
        for(auto n = oldBuffer[i]; n;) {
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
    return *this;
  }

  inline size_t getCapacity() const {
    return m_capacity;
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
    CompactUIntArray result(m + 1);
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

  class CompactMapIteratorEntry : public AbstractEntry {
  private:
     MapEntry<K,V> *m_entry;
  public:
    CompactMapIteratorEntry() {
      m_entry = nullptr;
    }
    inline CompactMapIteratorEntry &operator=(MapEntry<K,V> &entry) {
      m_entry = &entry;
      return *this;
    }
    const void *key()   const override {
      return &m_entry->m_key;
    }
    void *value()       const override {
      return &m_entry->m_value;
    }
  };

  class CompactMapEntryIterator : public AbstractIterator {
  private:
    CompactHashMap             &m_map;
    LinkObject<MapEntry<K,V> > *m_current, *m_next, **m_bufp, **m_endBuf;
    UINT64                      m_updateCount;

    void first() {
      m_current = nullptr;
      if(m_map.m_buffer) {
        m_endBuf = m_map.m_buffer + m_map.getCapacity();
        for(auto p = m_map.m_buffer; p < m_endBuf; p++) {
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

    inline void checkUpdateCount() const {
      if(m_updateCount != m_map.m_updateCount) {
        concurrentModificationError(_T("CompactMapEntryIterator"));
      }
    }
  protected:
    CompactMapIteratorEntry m_entry;

  public:
    CompactMapEntryIterator(const CompactHashMap *map) : m_map(*(CompactHashMap*)map), m_updateCount(map->m_updateCount) {
      first();
    }
    AbstractIterator *clone()         override {
      return new CompactMapEntryIterator(*this);
    }
    bool              hasNext() const override {
      return m_next != nullptr;
    }
    void             *next()          override {
      if(m_next == nullptr) {
        noNextElementError(_T("CompactMapEntryIterator"));
      }
      __assume(m_next);
      checkUpdateCount();
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
      m_entry = m_current->m_e;
      return &m_entry;
    }

    void              remove()        override {
      if(m_current == nullptr) {
        noCurrentElementError(_T("CompactMapEntryIterator"));
      }
      checkUpdateCount();
      __assume(m_current);
      m_map.remove(m_current->m_e.m_key);
      m_current = nullptr;
      m_updateCount = m_map.m_updateCount;
    }
  };

  class CompactMapKeyIterator : public CompactMapEntryIterator {
  public:
    CompactMapKeyIterator(const CompactHashMap *map) : CompactMapEntryIterator(map) {
    }
    AbstractIterator *clone()        override {
      return new CompactMapKeyIterator(*this);
    }
    void             *next()         override {
      __super::next();
      return (void*)m_entry.key();
    }
  };
  ConstIterator<Entry<K,V> > getIterator()    const override {
    return ConstIterator<Entry<K,V> >(new CompactMapEntryIterator(this));
  }
  Iterator<Entry<K,V> >      getIterator()          override {
    return Iterator<Entry<K,V> >(new CompactMapEntryIterator(this));
  }
  ConstIterator<K>           getKeyIterator() const override {
    return Iterator<K>(new CompactMapKeyIterator(this));
  }
  Iterator<K>                getKeyIterator()       override {
    return Iterator<K>(new CompactMapKeyIterator(this));
  }

  bool hasOrder()                             const override {
    return false;
  }

  bool operator==(const CompactHashMap &map) const {
    if(this == &map) return true;
    if(map.size() != size()) {
      return false;
    }
    for(auto it = getIterator(); it.hasNext();) {
      const Entry<K,V> &e = it.next();
      const V *mv = map.get(e.getKey());
      if((mv == nullptr) || (*mv != e.getValue())) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const CompactHashMap &map) const {
    return !(*this == map);
  }
};

#define DEFINECOMPACTHASHMAPTEMPLATE(ktype)                                                                                             \
template<typename T, UINT pageSize=20000> class Compact##ktype##HashMap : public CompactHashMap<Compact##ktype##KeyType, T, pageSize> { \
  public:                                                                                                                               \
  Compact##ktype##HashMap() {                                                                                                           \
  }                                                                                                                                     \
  explicit Compact##ktype##HashMap(size_t capacity) : CompactHashMap(capacity) {                                                        \
  }                                                                                                                                     \
  Compact##ktype##HashMap(const CompactHashMap &src) : CompactHashMap(src) {                                                            \
  }                                                                                                                                     \
  Compact##ktype##HashMap &operator=(const CompactHashMap &src) {                                                                       \
    __super::operator=(src);                                                                                                            \
    return *this;                                                                                                                       \
  }                                                                                                                                     \
}

DEFINECOMPACTHASHMAPTEMPLATE(Short );
DEFINECOMPACTHASHMAPTEMPLATE(UShort);
DEFINECOMPACTHASHMAPTEMPLATE(Int   );
DEFINECOMPACTHASHMAPTEMPLATE(UInt  );
DEFINECOMPACTHASHMAPTEMPLATE(Str   );
DEFINECOMPACTHASHMAPTEMPLATE(StrI  );
