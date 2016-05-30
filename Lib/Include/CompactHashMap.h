#pragma once

#include <Map.h>

#pragma pack(push,1)

#include "CompactKeyType.h"

template <class K, class V> class MapEntry {
public:
  K m_key;
  V m_value;
};

template <class K, class V> class CompactHashNode : public MapEntry<K, V> {
public:
  CompactHashNode<K, V> *m_next;
};

#pragma pack(pop)

template <class K, class V> class CompactHashNodePage {
public:
  unsigned int               m_count;
  CompactHashNodePage<K, V> *m_next;
  CompactHashNode<K, V>      m_nodes[20000];

  CompactHashNodePage() {
    clear();
  }

  void clear() {
    m_count = 0;
  }

  CompactHashNode<K, V> *fetchNode() {
    return &m_nodes[m_count++];
  }
  
  bool isFull() const {
    return m_count == ARRAYSIZE(m_nodes);
  }

  bool isEmpty() const {
    return m_count == 0;
  }

  void save(ByteOutputStream &s) const {
    s.putBytes((BYTE*)&m_count, sizeof(m_count));
    for(int i = 0; i < m_count;) {
      MapEntry<K, V> buffer[1000];
      const int n = min(m_count-i, ARRAYSIZE(buffer));
      for(int j = 0; j < n;) {
        buffer[j++] = m_nodes[i++];
      }
      s.putBytes((BYTE*)buffer, sizeof(buffer[0])*j);
    }
  }

  void load(ByteInputStream &s) {
    s.getBytesForced((BYTE*)&m_count, sizeof(m_count));
    for(int i = 0; i < m_count;) {
      MapEntry<K, V> buffer[1000];
      const int n = min(m_count-i, ARRAYSIZE(buffer));
      s.getBytesForced((BYTE*)buffer, sizeof(buffer[0])*n);
      for(int j = 0; j < n;) {
        (MapEntry<K, V>&)m_nodes[i++] = buffer[j++];
      }
    }
  }
};

template <class K, class V> class CompactHashMap {
private:
  unsigned int               m_size;
  unsigned int               m_capacity;
  CompactHashNode<K, V>    **m_buffer;
  CompactHashNodePage<K, V> *m_firstPage;

  CompactHashNode<K, V> *fetchNode() {
    if(m_firstPage == NULL || m_firstPage->isFull()) {
      CompactHashNodePage<K, V> *p = new CompactHashNodePage<K, V>;
      p->m_next   = m_firstPage;
      m_firstPage = p;
    }
    return m_firstPage->fetchNode();
  }

  CompactHashNodePage<K, V> *getFirstPage() {
    return m_firstPage;
  }

  CompactHashNode<K, V> **allocateBuffer(unsigned long capacity) {
    CompactHashNode<K, V> **result = capacity ? new CompactHashNode<K, V>*[capacity] : NULL;
    if(capacity) {
      memset(result, 0, sizeof(result[0])*capacity);
    }
    return result;
  }

  void init(unsigned long capacity) {
    m_size       = 0;
    m_capacity   = capacity;
    m_buffer     = allocateBuffer(capacity);
    m_firstPage  = NULL;
  }

  int getChainLength(unsigned long index) const {
    int count = 0;
    for(CompactHashNode<K, V> *p = m_buffer[index]; p; p = p->m_next) {
      count++;
    }
    return count;
  }

public:
  CompactHashMap() {
    init(0);
  }

  explicit CompactHashMap(unsigned long capacity) {
    init(capacity);
  }

  CompactHashMap(const CompactHashMap<K, V> &src) {
    init(src.m_capacity);
    addAll(src);
  }

  CompactHashMap<K, V> &operator=(const CompactHashMap<K, V> &src) {
    if(this == &src) {
      return *this;
    }
    clear();
    init(src.m_capacity);
    addAll(src);
    return *this;
  }

  ~CompactHashMap() {
    clear();
  }

  void setCapacity(unsigned long capacity) {
    if(capacity < m_size) {
      capacity = m_size;
    }
/*
    printf("\nHashMap:setCapacity(%d). old capacity=%d. size=%d\n",capacity,m_capacity,m_size);
    fflush(stdout);
*/
    if(m_buffer) {
      delete[] m_buffer;
      m_buffer = NULL;
    }

    m_capacity = capacity;
    m_buffer   = allocateBuffer(capacity);

    for(CompactHashNodePage<K, V> *page = m_firstPage; page; page = page->m_next) {
      for(int i = 0; i < (int)(page->m_count); i++) {
        CompactHashNode<K, V> *n = page->m_nodes+i;
        const unsigned long index = n->m_key.hashCode() % m_capacity;
        n->m_next = m_buffer[index];
        m_buffer[index] = n;
      }
    }
  }

  unsigned long getCapacity() const {
    return m_capacity;
  }

  int getPageCount() const {
    int count = 0;
    for(const CompactHashNodePage<K, V> *p = m_firstPage; p; p = p->m_next) {
      count++;
    }
    return count;
  }

  bool put(const K &key, const V &value) {
    unsigned long index;
    if(m_capacity) {
      index = key.hashCode() % m_capacity;
      for(CompactHashNode<K, V> *p = m_buffer[index]; p; p = p->m_next) {
        if(key == p->m_key) {
          return false;
        }
      }
    }
    if(m_size+1 > m_capacity*3) {
      setCapacity(m_size*5+5);
      index = key.hashCode() % m_capacity; // no need to search key again. if m_capacity was 0, the map is empty
    }
    CompactHashNode<K, V> *p = fetchNode();
    p->m_key                 = key;
    p->m_value               = value;
    p->m_next                = m_buffer[index];
    m_buffer[index]          = p;
    m_size++;
    return true;
  }

  V *get(const K &key) {
    if(m_capacity) {
      const unsigned long index = key.hashCode() % m_capacity;
      for(CompactHashNode<K, V> *p = m_buffer[index]; p; p = p->m_next) {
        if(key == p->m_key) {
          return &p->m_value;
        }
      }
    }
    return NULL;
  }

  const V *get(const K &key) const {
    if(m_capacity) {
      const unsigned long index = key.hashCode() % m_capacity;
      for(const CompactHashNode<K, V> *p = m_buffer[index]; p; p = p->m_next) {
        if(key == p->m_key) {
          return &p->m_value;
        }
      }
    }
    return NULL;
  }

  bool remove(const K &key) {
    throwException(_T("CompactHashMap::remove:Unsuppported operation"));
    return false;
  }

  void clear() {
    CompactHashNodePage<K, V> *p, *q;
    for(p = m_firstPage; p; p = q) {
      q = p->m_next;
      delete p;
    }
    m_firstPage = NULL;
    m_size      = 0;
    setCapacity(0);
  }

  unsigned __int64 size() const {
    return m_size;
  }

  bool isEmpty() const {
    return m_size == 0;
  }

  CompactIntArray getLength() const {
    CompactIntArray result;
    CompactIntArray tmp;
    const unsigned long capacity = getCapacity();
    int m = 0;
    for(unsigned long index = 0; index < capacity; index++) {
      int l = getChainLength(index);
      tmp.add(l);
      if(l > m) {
        m = l;
      }
    }
    for(int i = 0; i <= m; i++) {
      result.add(0);
    }
    for(unsigned long index = 0; index < capacity; index++) {
      result[tmp[index]]++;
    }
    return result;
  }

  int getMaxChainLength() const {
    int m = 0;
    const unsigned long capacity = getCapacity();
    for(unsigned long i = 0; i < capacity; i++) {
      int l = getChainLength(i);
      if(l > m) {
        m = l;
      }
    }
    return m;
  }

  class CompactMapKeyIterator : public AbstractIterator {
  private:
    CompactHashNodePage<K, V> *m_currentPage;
    unsigned int               m_currentIndex;

  public:
    CompactMapKeyIterator(CompactHashNodePage<K, V> *firstPage) {
      m_currentPage  = firstPage;
      m_currentIndex = 0;
    }

    AbstractIterator *clone() {
      return new CompactMapKeyIterator(*this);
    }

    bool hasNext() const {
      return m_currentPage != NULL;
    }

    void *next() {
      if(m_currentPage == NULL) {
        throwException(_T("CompactMapKeyIterator:next:No such element"));
      }
      K *result = &m_currentPage->m_nodes[m_currentIndex++].m_key;
      if(m_currentIndex == m_currentPage->m_count) {
        m_currentIndex = 0;
        m_currentPage = m_currentPage->m_next;
      }
      return result;
    }

    void remove() {
      throwException(_T("Unsupported operation"));
    }
  };

  Iterator<K> getKeyIterator() const {
    CompactHashMap<K, V> *tmp = (CompactHashMap<K, V>*)this;
    return Iterator<K>(new CompactMapKeyIterator(tmp->getFirstPage()));
  }

  class CompactMapIteratorEntry : public AbstractEntry {
  private:
    MapEntry<K, V> *m_entry;
  public:
    CompactMapIteratorEntry() {
      m_entry = NULL;
    }

    CompactMapIteratorEntry &operator=(MapEntry<K, V> &entry) {
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
    CompactHashNodePage<K, V> *m_currentPage;
    unsigned int               m_currentIndex;
    CompactMapIteratorEntry    m_buffer;

  public:
    CompactMapEntryIterator(CompactHashNodePage<K, V> *firstPage) {
      m_currentPage  = firstPage;
      m_currentIndex = 0;
    }

    AbstractIterator *clone() {
      return new CompactMapEntryIterator(*this);
    }

    bool hasNext() const {
      return m_currentPage != NULL;
    }

    void *next() {
      if(m_currentPage == NULL) {
        noNextElementError(_T("CompactMapEntryIterator"));
      }
      m_buffer = m_currentPage->m_nodes[m_currentIndex++];
      if(m_currentIndex == m_currentPage->m_count) {
        m_currentIndex = 0;
        m_currentPage = m_currentPage->m_next;
      }
      return &m_buffer;
    }

    void remove() {
      unsupportedOperationError(_T("CompactMapEntryIterator"));
    }
  };

  Iterator<Entry<K, V> > getEntryIterator() const {
    CompactHashMap<K, V> *tmp = (CompactHashMap<K, V>*)this;
    return Iterator<Entry<K, V> >(new CompactMapEntryIterator(tmp->getFirstPage()));
  }

  void save(ByteOutputStream &s) const {
    const int pageCount = getPageCount();
    const int kSize = sizeof(K);
    const int vSize = sizeof(V);
    s.putBytes((BYTE*)&kSize, sizeof(kSize));
    s.putBytes((BYTE*)&vSize, sizeof(vSize));
    s.putBytes((BYTE*)&pageCount, sizeof(pageCount));
    for(const CompactHashNodePage<K, V> *p = m_firstPage; p; p = p->m_next) {
      p->save(s);
    }
  }

  void load(ByteInputStream &s) {
    clear();
    int kSize, vSize;
    s.getBytesForced((BYTE*)&kSize,sizeof(kSize));
    s.getBytesForced((BYTE*)&vSize,sizeof(vSize));
    if((kSize != sizeof(K)) ||| (vSize != sizeof(V))) {
      throwException(_T("sizeof(Key), sizeof(Value):(%d,%d). Sizes from stream:(%d,%d)"), sizeof(K), sizeof(V), kSize, vSize);
    }
    int pageCount;
    s.getBytesForced((BYTE*)&pageCount,sizeof(pageCount));
    CompactHashNodePage<K, V> **pp = &m_firstPage;
    for(int i = 0; i < pageCount; i++, pp = &(*pp)->m_next) { // append them, so we get them in the same order as they were when saved
      CompactHashNodePage<K, V> *newPage = new CompactHashNodePage<K, V>;
      newPage->load(s);
      m_size += newPage->m_count;
      *pp = newPage;
    }
    *pp = NULL;
    setCapacity(m_size);
  }

  void unload() {
    clear();
  }
};
