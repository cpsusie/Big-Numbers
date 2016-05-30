#pragma once

#include <Set.h>

#pragma pack(push,1)

#include "CompactKeyType.h"

template <class K> class SetEntry {
public:
  K m_key;
};

template <class K> class CompactHashElement : public SetEntry<K> {
public:
  CompactHashElement<K> *m_next;
};

#pragma pack(pop)

template <class K> class CompactHashElementPage {
public:
  unsigned int               m_count;
  CompactHashElementPage<K> *m_next;
  CompactHashElement<K>      m_elements[20000];

  CompactHashElementPage() {
    clear();
  }

  void clear() {
    m_count = 0;
  }

  CompactHashElement<K> *fetchNode() {
    return &m_elements[m_count++];
  }
  
  bool isFull() const {
    return m_count == ARRAYSIZE(m_elements);
  }

  bool isEmpty() const {
    return m_count == 0;
  }

  void save(ByteOutputStream &s) const {
    s.putBytes((BYTE*)&m_count,sizeof(m_count));
    for(int i = 0; i < m_count;) {
      SetEntry<K> buffer[1000];
      const int n = min(m_count-i,ARRAYSIZE(buffer));
      for(int j = 0; j < n;) {
        buffer[j++] = m_elements[i++];
      }
      s.putBytes((BYTE*)buffer,sizeof(buffer[0])*j);
    }
  }

  void load(ByteInputStream &s) {
    s.getBytesForced((BYTE*)&m_count,sizeof(m_count));
    for(int i = 0; i < m_count;) {
      SetEntry<K> buffer[1000];
      const int n = min(m_count-i,ARRAYSIZE(buffer));
      s.getBytesForced((BYTE*)buffer,sizeof(buffer[0])*n);
      for(int j = 0; j < n;) {
        (SetEntry<K>&)m_elements[i++] = buffer[j++];
      }
    }
  }
};

template <class K> class CompactHashSet {
private:
  unsigned int               m_size;
  unsigned int               m_capacity;
  CompactHashElement<K>    **m_buffer;
  CompactHashElementPage<K> *m_firstPage;

  CompactHashElement<K> *fetchNode() {
    if(m_firstPage == NULL || m_firstPage->isFull()) {
      CompactHashElementPage<K> *p = new CompactHashElementPage<K>;
      p->m_next   = m_firstPage;
      m_firstPage = p;
    }
    return m_firstPage->fetchNode();
  }

  CompactHashElementPage<K> *getFirstPage() {
    return m_firstPage;
  }

  CompactHashElement<K> **allocateBuffer(unsigned long capacity) {
    CompactHashElement<K> **result = capacity ? new CompactHashElement<K>*[capacity] : NULL;
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
    for(CompactHashElement<K> *p = m_buffer[index]; p; p = p->m_next) {
      count++;
    }
    return count;
  }

public:
  CompactHashSet() {
    init(0);
  }

  explicit CompactHashSet(unsigned long capacity) {
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

    for(CompactHashElementPage<K> *page = m_firstPage; page; page = page->m_next) {
      for(unsigned int i = 0; i < page->m_count; i++) {
        CompactHashElement<K> *n = page->m_elements+i;
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
    for(const CompactHashElementPage<K> *p = m_firstPage; p; p = p->m_next) {
      count++;
    }
    return count;
  }

  bool add(const K &key) {
    unsigned long index;
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
    CompactHashElement<K> *p = fetchNode();
    p->m_key                 = key;
    p->m_next                = m_buffer[index];
    m_buffer[index]          = p;
    m_size++;
    return true;
  }

  bool contains(const K &key) const {
    if(m_capacity) {
      const unsigned long index = key.hashCode() % m_capacity;
      for(const CompactHashElement<K> *p = m_buffer[index]; p; p = p->m_next) {
        if(key == p->m_key) {
          return true;
        }
      }
    }
    return false;
  }

  bool remove(const K &key) {
    throwException(_T("CompactHashSet::remove:Unsuppported operation"));
    return false;
  }

  void clear() {
    CompactHashElementPage<K> *p, *q;
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

  bool addAll(const CompactHashSet<K> &src) {
    bool changed = false;
    for(Iterator<K> it = src.getIterator(); it.hasNext(); ) {
      if(add(it.next())) {
        changed = true;
      }
    }
    return changed;
  }

  class CompactSetIterator : public AbstractIterator {
  private:
    CompactHashElementPage<K> *m_currentPage;
    int                        m_currentIndex;

  public:
    CompactSetIterator(CompactHashElementPage<K> *firstPage) {
      m_currentPage  = firstPage;
      m_currentIndex = 0;
    }

    AbstractIterator *clone() {
      return new CompactSetIterator(*this);
    }

    bool hasNext() const {
      return m_currentPage != NULL;
    }

    void *next() {
      if(m_currentPage == NULL) {
        noNextElementError(_T("CompactSetIterator"));
      }
      K *result = &m_currentPage->m_elements[m_currentIndex++].m_key;
      if(m_currentIndex == m_currentPage->m_count) {
        m_currentIndex = 0;
        m_currentPage = m_currentPage->m_next;
      }
      return result;
    }

    void remove() {
      unsupportedOperationError(_T("CompactHashSet"));
    }
  };

  Iterator<K> getIterator() const {
    CompactHashSet<K> *tmp = (CompactHashSet<K>*)this;
    return Iterator<K>(new CompactSetIterator(tmp->getFirstPage()));
  }

  void save(ByteOutputStream &s) const {
    const int pageCount = getPageCount();
    const int kSize     = sizeof(K);
    s.putBytes((BYTE*)&kSize    , sizeof(kSize));
    s.putBytes((BYTE*)&pageCount, sizeof(pageCount));
    for(const CompactHashElementPage<K> *p = m_firstPage; p; p = p->m_next) {
      p->save(s);
    }
  }

  void load(ByteInputStream &s) {
    clear();
    int kSize;
    s.getBytesForced((BYTE*)&kSize, sizeof(kSize));
    if(kSize != sizeof(K)) {
      throwException(_T("sizeof(Key):%d. Size from stream=%d"), sizeof(K), kSize);
    }
    int pageCount;
    s.getBytesForced((BYTE*)&pageCount, sizeof(pageCount));
    CompactHashElementPage<K> **pp = &m_firstPage;
    for(int i = 0; i < pageCount; i++, pp = &(*pp)->m_next) { // append them, so we get them in the same order as they were when saved
      CompactHashElementPage<K> *newPage = new CompactHashElementPage<K>;
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
