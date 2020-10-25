#include "pch.h"
#include <PackedArray.h>

class PackedArrayIterator : public AbstractIterator {
private:
  DECLARECLASSNAME;
  PackedArray &m_a;
  UINT         m_currentValue;
  UINT64       m_next;
  INT64        m_current;
  size_t       m_updateCount;
  PackedArrayIterator(PackedArrayIterator *src)
    : m_a(src->m_a)
    , m_currentValue(src->m_currentValue)
    , m_next(        src->m_next        )
    , m_current(     src->m_current     )
    , m_updateCount( src->m_updateCount )
  {
  }
  void checkUpdateCount() const {
    if(m_updateCount != m_a.getUpdateCount()) {
      concurrentModificationError(s_className);
    }
  }
public:
  PackedArrayIterator(const PackedArray *a) : m_a(*(PackedArray*)a) {
    m_currentValue =  0;
    m_next         =  0;
    m_current      = -1;
    m_updateCount  = m_a.getUpdateCount();
  }
  AbstractIterator *clone()         override {
    return new PackedArrayIterator(this);
  }
  bool              hasNext() const override {
    return m_next < m_a.size();
  }
  void             *next()          override;
  void              remove()        override;
};

DEFINECLASSNAME(PackedArrayIterator);

void *PackedArrayIterator::next() {
  if(m_next >= m_a.size()) {
    noNextElementError(s_className);
  }
  checkUpdateCount();
  m_currentValue = m_a.get(m_current = m_next++);
  return &m_currentValue;
}

void PackedArrayIterator::remove() {
  if(m_current < 0) {
    noCurrentElementError(s_className);
  }
  checkUpdateCount();
  m_a.remove(m_current,1);
  m_current     = -1;
  m_updateCount = m_a.getUpdateCount();
}

ConstIterator<UINT> PackedArray::getIterator() const {
  return ConstIterator<UINT>(new PackedArrayIterator(this));
}

// --------------------------------------- PackedArrayReverseIterator ----------------------------------------


class PackedArrayReverseIterator : public AbstractIterator {
private:
  DECLARECLASSNAME;
  PackedArray &m_a;
  UINT         m_currentValue;
  INT64        m_next;
  INT64        m_current;
  size_t       m_updateCount;
  PackedArrayReverseIterator(PackedArrayReverseIterator *src)
    : m_a(           src->m_a           )
    , m_currentValue(src->m_currentValue)
    , m_next(        src->m_next        )
    , m_current(     src->m_current     )
    , m_updateCount( src->m_updateCount )
  {
  }
  void checkUpdateCount() const {
    if(m_updateCount != m_a.getUpdateCount()) {
      concurrentModificationError(s_className);
    }
  }
public:
  PackedArrayReverseIterator(const PackedArray *a) : m_a(*(PackedArray*)a) {
    m_currentValue =  0;
    m_next         =  (INT64)m_a.size() - 1;
    m_current      = -1;
    m_updateCount  = m_a.getUpdateCount();
  }
  AbstractIterator *clone()         override {
    return new PackedArrayReverseIterator(this);
  }
  bool              hasNext() const override {
    return m_next >= 0;
  }
  void              *next()         override;
  void               remove()       override;
};

DEFINECLASSNAME(PackedArrayReverseIterator);

void *PackedArrayReverseIterator::next() {
  if(m_next < 0) {
    noNextElementError(s_className);
  }
  checkUpdateCount();
  m_currentValue = m_a.get(m_current = m_next--);
  return &m_currentValue;
}

void PackedArrayReverseIterator::remove() {
  if(m_current < 0) {
    noCurrentElementError(s_className);
  }
  checkUpdateCount();
  m_a.remove(m_current,1);
  m_current     = -1;
  m_updateCount = m_a.getUpdateCount();
}

ConstIterator<UINT> PackedArray::getReverseIterator() const {
  return ConstIterator<UINT>(new PackedArrayReverseIterator(this));
}
