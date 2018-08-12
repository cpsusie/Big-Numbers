#include "stdafx.h"

class PointSet : public BitSet {
private:
  CRect m_rect;
  int   m_width;
  static inline UINT getPixelCount(const CRect &r) {
    return getArea(r);
  }
  inline UINT getIndex(const CPoint &p) const {
    return (p.y-m_rect.top) * m_width + (p.x-m_rect.left);
  }
  inline CPoint getPoint(size_t index) const {
    return CPoint((int)(index % m_width + m_rect.left), (int)(index / m_width + m_rect.top));
  }
  friend class PointSetIterator;
public:
  PointSet(const CRect &r)
    : BitSet(getPixelCount(r))
    , m_rect(r)
    , m_width(r.Width())
  {
  }
  inline void add(const CPoint &p) {            // assume p is inside CRect
    BitSet::add(getIndex(p));
  }
  inline bool contains(const CPoint &p) const { // assume p is inside CRect
    return BitSet::contains(getIndex(p));
  }
  inline const CRect &getRect() const {
    return m_rect;
  }
  Iterator<CPoint> getIterator() const;
};
