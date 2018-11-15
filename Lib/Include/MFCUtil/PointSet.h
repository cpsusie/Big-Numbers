#pragma once

#include <BitSet.h>

class PointSet : private BitMatrix {
private:
  friend class PointSetIterator;
  CPoint m_topLeft;
  inline CPoint getPoint(const MatrixIndex &index) const {
    return CPoint((int)(m_topLeft.x+index.c), (int)(m_topLeft.y+index.r));
  }
  void checkSameRect(const TCHAR *method, const PointSet &s) const;
  String getRectString() const {
    return format(_T("topleft:%s, size:%s"), ::toString(topLeft()).cstr(), ::toString(size()).cstr());
  }
public:
  PointSet(const CRect &r)
    : BitMatrix(r.Height(), r.Width())
    , m_topLeft(r.TopLeft())
  {
  }
  inline void add(const CPoint &p) {            // assume p is inside CRect
    __super::set(p.y-m_topLeft.y,p.x-m_topLeft.x,true);
  }
  inline bool contains(const CPoint &p) const { // assume p is inside CRect
    return __super::get(p.y-m_topLeft.y,p.x-m_topLeft.x);
  }
  inline void clear() {
    __super::clear();
  }
  inline const CPoint &topLeft() const {
    return m_topLeft;
  }
  inline const CSize size() const {
    const MatrixDimension &dim = getDimension();
    return CSize((INT)dim.rowCount, (INT)dim.columnCount);
  }
  inline const CRect getRect() const {
    return CRect(m_topLeft, size());
  }
  PointSet &operator&=(const PointSet &s);
  PointSet &operator|=(const PointSet &s);
  PointSet &operator-=(const PointSet &s);
  PointSet &operator^=(const PointSet &s);
  Iterator<CPoint> getIterator() const;
  String toString() const;
};

