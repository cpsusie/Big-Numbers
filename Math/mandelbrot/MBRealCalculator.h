#pragma once

#include "MBCalculator.h"

inline CPoint toCPoint(const RealPoint2D &p) {
  return CPoint(getInt(p.x), getInt(p.y));
}

inline RealPoint2D toRealPoint(const CPoint &p) {
  return RealPoint2D(p.x, p.y);
}

inline CRect toCRect(const RealRectangle2D &r) {
  return CRect(getInt(r.m_x),getInt(r.m_y),getInt(r.m_x+r.m_w),getInt(r.m_y+r.m_h));
}

inline RealRectangle2D toRealRect(const CRect &r) {
  return RealRectangle2D(r.left,r.top,r.Width(),r.Height());
}

class MBRealCalculator : public MBCalculator {
private:
  const RealIntervalTransformation *m_xtr, *m_ytr;
  UINT               findCountPaintOrbit(const Real    &X, const Real        &Y  , UINT maxCount);
  CellCountAccessor *followBlackEdge(    const CPoint  &p, CellCountAccessor *cca, UINT maxCount);
  inline CPoint      toCPoint(           const Real    &x, const Real        &y) const {
    return CPoint(getInt(m_xtr->forwardTransform(x)), getInt(m_ytr->forwardTransform(y)));
  }
public:
  MBRealCalculator(CalculatorPool *pool, int id) : MBCalculator(pool, id) {
  }
  UINT run();
};
