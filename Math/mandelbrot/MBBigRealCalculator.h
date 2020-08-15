#pragma once

#include <Math/BigReal/BigRealResourcePool.h>
#include "MBCalculator.h"

inline CPoint toCPoint(const BigRealPoint2D &p) {
  return CPoint((int)p.x, (int)p.y);
}

inline BigRealPoint2D toBigRealPoint(const Point2D &p, DigitPool *digitPool) {
  BigRealPoint2D result(digitPool);
  return result = p;
}

inline CRect toCRect(const BigRealRectangle2D &r) {
  return CRect((int)r.getX(),(int)r.getY(),(int)(r.getX()+r.getWidth()),(int)(r.getY()+r.getHeight()));
}

inline BigRealRectangle2D toBigRealRect(const Rectangle2D &r, DigitPool *digitPool) {
  BigRealRectangle2D result(digitPool);
  return result = r;
}

class MBBigRealCalculator : public MBCalculator {
private:
  static const BigRealIntervalTransformation *s_xtr, *s_ytr;
  static Array<BigReal>                       s_xValue, s_yValue; // indexed by CPoint p (p.x,p.y) giving (x,y)-point in tr.toRectangle()

  DigitPool *m_digitPool;
  size_t     m_digits;
  BigReal    _4;
  UINT               findCountPaintOrbit(const BigReal &X, const BigReal     &Y  , UINT maxCount);
  UINT               findCountFast(      const BigReal &X, const BigReal     &Y  , UINT maxCount);
  CellCountAccessor *followBlackEdge(    const CPoint  &p, CellCountAccessor *cca, UINT maxCount);
  inline CPoint      toCPoint(           const BigReal &x, const BigReal     &y) const {
    return CPoint((int)s_xtr->forwardTransform(x), (int)s_ytr->forwardTransform(y));
  }
  inline DigitPool *getDigitPool() const {
    return m_digitPool;
  }
public:
  MBBigRealCalculator(CalculatorPool *pool, int id)
    : MBCalculator(pool, id)
    , _4(4,BigRealResourcePool::fetchDigitPool())
  {
    m_digitPool = _4.getDigitPool();
  }
  ~MBBigRealCalculator() {
    _4.setToZero(); // no more digits in use by digitPool
    BigRealResourcePool::releaseDigitPool(m_digitPool);
  }
  static void prepareMaps(const BigRealRectangleTransformation &tr);
  static void cleanupMaps();
  UINT run();
};
