#pragma once

#include "MBCalculator.h"

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
