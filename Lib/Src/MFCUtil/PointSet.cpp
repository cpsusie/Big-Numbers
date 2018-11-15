#include "pch.h"
#include <MFCUtil/PointSet.h>

void PointSet::checkSameRect(const TCHAR *method, const PointSet &s) const {
  if(getRect() != s.getRect()) {
    throwInvalidArgumentException(method
                                 ,_T("PointSets must have the same rect. this->%s, s.%s")
                                 ,getRectString().cstr(), s.getRectString().cstr()
                                 );
  }
}

PointSet &PointSet::operator&=(const PointSet &s) {
  checkSameRect(__TFUNCTION__,s);
  __super::operator&=(s);
  return *this;
}

PointSet &PointSet::operator|=(const PointSet &s) {
  checkSameRect(__TFUNCTION__,s);
  __super::operator|=(s);
  return *this;
}

PointSet &PointSet::operator-=(const PointSet &s) {
  checkSameRect(__TFUNCTION__,s);
  __super::operator-=(s);
  return *this;
}

PointSet &PointSet::operator^=(const PointSet &s) {
  checkSameRect(__TFUNCTION__,s);
  __super::operator^=(s);
  return *this;
}

class PointSetIterator : public AbstractIterator {
private:
  PointSet             &m_set;
  Iterator<MatrixIndex> m_it;
  CPoint                m_p;
  PointSetIterator(const PointSetIterator &it) : m_set(it.m_set), m_it(it.m_it) {
  }
public:
  PointSetIterator(const PointSet &set) : m_set((PointSet&)set), m_it(((BitMatrix&)set).getIterator()) {
  }
  AbstractIterator *clone() {
    return new PointSetIterator(*this);
  }
  bool hasNext() const {
    return m_it.hasNext();
  }
  void *next() {
    m_p = m_set.getPoint(m_it.next());
    return &m_p;
  }
  void remove() {
    m_it.remove();
  }
};

Iterator<CPoint> PointSet::getIterator() const {
  return Iterator<CPoint>(new PointSetIterator(*this));
}

String PointSet::toString() const {
  String result;
  Iterator<CPoint> it = getIterator();
  for(int i = 0; (i < 20) && it.hasNext(); i++) {
    const CPoint &p = it.next();
    result += format(_T("(%d,%d) "), p.x,p.y);
  }
  return result;
}

