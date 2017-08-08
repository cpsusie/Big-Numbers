#pragma once

// Assume T has a public field T *m_next
template<class T> class HeapObjectPool {
private:
  class HeapObjectPage {
  private:
    HeapObjectPage *m_nextPage;
    T               m_object[20000];

  public:
    HeapObjectPage(HeapObjectPage *nextPage) {
      m_nextPage = nextPage;
    }

    T *makeChain() {
      for(T *p = &LASTVALUE(m_object), *next = NULL; p >= m_object; next = p, p--) {
        p->m_next = next;
      }
      return m_object;
    }

    HeapObjectPage *nextPage() const {
      return m_nextPage;
    }
  };

  HeapObjectPage *m_firstPage;
  T              *m_freeList;

public:
  HeapObjectPool() : m_firstPage(NULL), m_freeList(NULL) {
  }

  virtual ~HeapObjectPool() {
    releaseAll();
  }

  inline T *fetch() {
    if(m_freeList == NULL) {
      m_firstPage = new HeapObjectPage(m_firstPage); TRACE_NEW(m_firstPage);
      m_freeList = m_firstPage->makeChain();
    }
    T *result = m_freeList;
    m_freeList = m_freeList->m_next;
    return result;
  }

  inline void release(T *e) {
    e->m_next = m_freeList;
    m_freeList = e;
  }

  void releaseAll() {
    HeapObjectPage *p, *q;
    for(p = m_firstPage; p; p = q) {
      q = p->nextPage();
      SAFEDELETE(p);
    }
    m_firstPage = NULL;
    m_freeList  = NULL;
  }

  int getPageCount() const {
    int count = 0;
    for(const HeapObjectPage *p = m_firstPage; p; p = p->m_next) {
      count++;
    }
    return count;
  }
};

#pragma pack(push,1)

template <class T> class LinkElement {
public:
  LinkElement *m_next;
  T            m_e;
};

#pragma pack(pop)
