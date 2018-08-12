#pragma once

#include "MBCalculator.h"

inline CPoint toCPoint(const BigRealPoint2D &p) {
  return CPoint(getInt(p.x), getInt(p.y));
}

inline CRect toCRect(const BigRealRectangle2D &r) {
  return CRect(getInt(r.getX()),getInt(r.getY()),getInt(r.getX()+r.getWidth()),getInt(r.getY()+r.getHeight()));
}

inline BigRealPoint2D toBigRealPoint(const Point2DP &p, DigitPool *digitPool) {
  BigRealPoint2D result(digitPool);
  return result = p;
}
inline BigRealRectangle2D toBigRealRect(const Rectangle2DR &r, DigitPool *digitPool) {
  BigRealRectangle2D result(digitPool);
  return result = r;
}

class MBBigRealCalculator : public MBCalculator {
private:
  DigitPool *m_digitPool;
  size_t     m_digits;
  BigReal    _4;
  const BigRealIntervalTransformation  *m_xtr, *m_ytr;
  UINT           findITCountPaintOrbit(const BigReal &X, const BigReal &Y , UINT maxIteration);
  UINT           findITCountFast(      const BigReal &X, const BigReal &Y , UINT maxIteration);
  PixelAccessor *followBlackEdge(      const CPoint  &p, PixelAccessor *pa, UINT maxIteration);
  inline CPoint toCPoint(              const BigReal &x, const BigReal &y) const {
    return CPoint(getInt(m_xtr->forwardTransform(x)), getInt(m_ytr->forwardTransform(y)));
  }
  inline DigitPool *getDigitPool() const {
    return m_digitPool;
  }
protected:
  inline UINT findItCount(const BigReal &X, const BigReal &Y, UINT maxIteration) {
    return isWithOrbit() ? findITCountPaintOrbit(X,Y,maxIteration)
                         : findITCountFast(      X,Y,maxIteration);
  };
public:
  MBBigRealCalculator(CalculatorPool *pool, int id) 
    : MBCalculator(pool, id)
    , _4(4,BigRealResourcePool::getInstance().fetchDigitPool())
  {
    m_digitPool = _4.getDigitPool();
  }
  ~MBBigRealCalculator() {
    _4.setToZero(); // no more digits in use by digitPool
    BigRealResourcePool::getInstance().releaseDigitPool(m_digitPool);
  }
  UINT run();
};
