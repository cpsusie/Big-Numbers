#include "StdAfx.h"
#include "PointSet.h"

class PointSetIterator : public AbstractIterator {
private:
  const PointSet  &m_ps;
  Iterator<size_t> m_it;
  CPoint           m_point;
public:
  PointSetIterator(const PointSet &ps) : m_ps(ps) {
    m_it = ((BitSet&)m_ps).getIterator();
  }
  AbstractIterator *clone() {
    return new PointSetIterator(m_ps);
  }
  bool hasNext() const {
    return m_it.hasNext();
  }
  void *next() {
    m_point = m_ps.getPoint(m_it.next());
    return &m_point;
  }
  void remove() {
    unsupportedOperationError(__TFUNCTION__);
  }
};

Iterator<CPoint> PointSet::getIterator() const {
  return Iterator<CPoint>(new PointSetIterator(*this));
}
