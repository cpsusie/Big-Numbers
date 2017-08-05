#pragma once

// Assume T has a public field T *m_next
template<class T> class HeapElementPool {
private:
  class HeapElementPage {
  private:
    HeapElementPage *m_nextPage;
    T                m_elements[20000], *m_freeList;

    void makeChain() {
      memset(m_elements,0,sizeof(m_elements));
      for(T *p = &LASTVALUE(m_elements), *next = NULL; p >= m_elements; next = p, p--) {
        p->m_next = next;
      }
      m_freeList = m_elements;
    }

  public:
    HeapElementPage(HeapElementPage *nextPage) {
      makeChain();
      m_nextPage = nextPage;
    }

    // Assume m_freeList != NULL;
    inline T *fetchElement() {
      T *result = m_freeList;
      m_freeList = m_freeList->m_next;
      return result;
    }

    inline void releaseElement(T *n) {
      n->m_next = m_freeList;
      m_freeList = n;
    }

    inline bool isFull() const {
      return m_freeList == NULL;
    }

    HeapElementPage *nextPage() const {
      return m_nextPage;
    }
  };

  HeapElementPage *m_firstPage;

public:
  HeapElementPool() : m_firstPage(NULL) {
  }

  virtual ~HeapElementPool() {
    releaseAll();
  }

  inline T *fetchElement() {
    if(m_firstPage == NULL || m_firstPage->isFull()) {
      m_firstPage = new HeapElementPage(m_firstPage); TRACE_NEW(m_firstPage);
    }
    return m_firstPage->fetchElement();
  }

  inline void releaseElement(T *e) {
    m_firstPage->releaseElement(e);
  }

  void releaseAll() {
    HeapElementPage *p, *q;
    for(p = m_firstPage; p; p = q) {
      q = p->nextPage();
      SAFEDELETE(p);
    }
    m_firstPage = NULL;
  }

  int getPageCount() const {
    int count = 0;
    for(const HeapElementPage *p = m_firstPage; p; p = p->m_next) {
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
