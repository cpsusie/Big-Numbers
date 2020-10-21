#pragma once

#include "DebugLog.h"

// Assume T has a public field T *m_next
template<typename T, UINT pageSize=20000> class HeapObjectPool {
private:
  class HeapObjectPage {
  private:
    HeapObjectPage *m_nextPage;
    T               m_object[pageSize];

  public:
    HeapObjectPage(HeapObjectPage *nextPage) {
//      DEBUGLOG(_T("%s:pageSize:%5u(%8s bytes), elementsize:%zu, nextPage:%p\n"), __TFUNCTION__, pageSize, format1000(sizeof(HeapObjectPage)).cstr(), sizeof(T), nextPage);
      m_nextPage = nextPage;
    }

    T *makeChain() {
      for(T *p = &LASTVALUE(m_object), *next = nullptr; p >= m_object; next = p, p--) {
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
  HeapObjectPool() : m_firstPage(nullptr), m_freeList(nullptr) {
  }

  virtual ~HeapObjectPool() {
    releaseAll();
  }

  inline T *fetch() {
    if(m_freeList == nullptr) {
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
    m_firstPage = nullptr;
    m_freeList  = nullptr;
  }

  int getPageCount() const {
    int count = 0;
    for(const HeapObjectPage *p = m_firstPage; p; p = p->m_next) {
      count++;
    }
    return count;
  }
};

template <typename T> class LinkObject {
public:
  LinkObject *m_next;
  T           m_e;
};
