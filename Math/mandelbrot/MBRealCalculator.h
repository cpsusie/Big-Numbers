#pragma once

#include "MBCalculator.h"

inline CPoint toCPoint(const RealPoint2D &p) {
  return CPoint((int)p.x, (int)p.y);
}

inline RealPoint2D toRealPoint(const CPoint &p) {
  return RealPoint2D(p.x, p.y);
}

inline CRect toCRect(const RealRectangle2D &r) {
  return CRect((int)r.m_x,(int)r.m_y,(int)(r.m_x+r.m_w),(int)(r.m_y+r.m_h));
}

inline RealRectangle2D toRealRect(const CRect &r) {
  return RealRectangle2D(r.left,r.top,r.Width(),r.Height());
}

class MBRealCalculator : public MBCalculator {
private:
  static const RealIntervalTransformation *s_xtr   , *s_ytr;
  static Real                             *s_xValue, *s_yValue; // indexed by CPoint p (p.x,p.y) giving (x,y)-point in tr.toRectangle()
  UINT               findCountPaintOrbit(const Real    &X, const Real        &Y  , UINT maxCount);
  CellCountAccessor *followBlackEdge(    const CPoint  &p, CellCountAccessor *cca, UINT maxCount);
  inline CPoint      toCPoint(           const Real    &x, const Real        &y) const {
    return CPoint((int)s_xtr->forwardTransform(x), (int)s_ytr->forwardTransform(y));
  }
public:
  MBRealCalculator(CalculatorPool *pool, int id) : MBCalculator(pool, id) {
  }
  static void prepareMaps(const RealRectangleTransformation &tr);
  static void cleanupMaps();
  UINT run();
};
