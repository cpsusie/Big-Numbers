#include "pch.h"
#include <ByteArray.h>

class ByteArrayIterator : public AbstractIterator {
private:
  ByteArray    &m_a;
  size_t        m_next;
  intptr_t      m_current;
  size_t        m_updateCount;
  inline void checkUpdateCount(const TCHAR *method) const {
    if(m_updateCount != m_a.getUpdateCount()) {
      concurrentModificationError(method);
    }
  }
public:
  ByteArrayIterator(const ByteArray *a) : m_a(*(ByteArray*)a) {
    m_next        = 0;
    m_current     = -1;
    m_updateCount = m_a.getUpdateCount();
  }
  AbstractIterator *clone()          final {
    return new ByteArrayIterator(*this);
  }
  inline bool hasNext()        const final {
    return m_next < m_a.size();
  }
  void *next()                       final {
    if(m_next >= m_a.size()) {
      noNextElementError(__TFUNCTION__);
    }
    checkUpdateCount(__TFUNCTION__);
    m_current = m_next++;
    return &m_a[m_current];
  }
  void remove()                      final {
    if(m_current < 0) {
      noCurrentElementError(__TFUNCTION__);
    }
    checkUpdateCount(__TFUNCTION__);
    m_a.remove(m_current,1);
    m_current     = -1;
    m_updateCount = m_a.getUpdateCount();
  }
};

Iterator<BYTE>      ByteArray::getIterator() {
  return Iterator<BYTE>(new ByteArrayIterator(this));
}

ConstIterator<BYTE> ByteArray::getIterator() const {
  return ConstIterator<BYTE>(new ByteArrayIterator(this));
}
