#pragma once

#include <Math/Rectangle2DTransformation.h>
#include "BigRealInterval.h"

#define AUTOPRECISION 0

class BigRealSize2D : public SizeTemplate<BigReal, 2> {
private:
public:
  template<typename X, typename Y> BigRealSize2D(const X &cx, const Y &cy, DigitPool *digitPool=nullptr)
    : SizeTemplate(digitPool?BigReal(cx,digitPool):cx)
                  ,digitPool?BigReal(cy,digitPool),digitPool:cx.getDigitPool()))
  {
  }
  inline BigRealSize2D(const BigRealSize2D &src, DigitPool *digitPool=nullptr)
    : SizeTemplate(BigReal(src[0],digitPool?digitPool:src.getDigitPool())
                  ,BigReal(src[1],digitPool?digitPool:src.getDigitPool()))
  {
  }
  inline DigitPool *getDigitPool() const {
    return cx().getDigitPool();
  }
  template<typename T> BigRealSize2D &operator=(const FixedDimensionVector<T, 2> &src) {
    DigitPool *dp = getDigitPool();
    cx() = BigReal(src.cx(),dp);
    cy() = BigReal(src.cy(),dp);
    return *this;
  }
#if defined(__ATLTYPES_H__)
  inline BigRealSize2D(const CSize &s, DigitPool *digitPool=nullptr)
    : Size2DTemplate(BigReal(s.cx,digitPool),BigReal(s.cy,digitPool))
  {
  }
  inline BigRealSize2D &operator=(const CSize &s) {
    cx() = s.cx; cy() = s.cy;
    return *this;
  }
  inline explicit operator CSize() const {
    return CSize((int)round(cx()), (int)round(cy()));
  }
#endif // __ATLTYPES_H__

};

class BigRealPoint2D : public Point2DTemplate<BigReal> {
public:
  inline BigRealPoint2D(DigitPool *digitPool=nullptr)
    : Point2DTemplate(BigReal(0,digitPool), BigReal(0,digitPool))
  {
  }
  inline BigRealPoint2D(const BigReal &x, const BigReal &y, DigitPool *digitPool=nullptr)
    : Point2DTemplate(BigReal(x,digitPool?digitPool:x.getDigitPool())
                     ,BigReal(y,digitPool?digitPool:x.getDigitPool()))
  {
  }
  inline BigRealPoint2D(const BigRealPoint2D &src, DigitPool *digitPool=nullptr)
    : Point2DTemplate(BigReal(src.x(),digitPool?digitPool:src.getDigitPool())
                     ,BigReal(src.y(),digitPool?digitPool:src.getDigitPool()))
  {
  }
  inline DigitPool *getDigitPool() const {
    return x().getDigitPool();
  }
  template<typename T> BigRealSize2D &operator=(const FixedDimensionVector<T, 2> &src) {
    DigitPool *dp = getDigitPool();
    cx() = BigReal(src.cx(),dp);
    cy() = BigReal(src.cy(),dp);
    return *this;
  }
  template<typename T> inline explicit operator Point2DTemplate<T>() const {
    return Point2DTemplate<T>((T)cx(), (T)cy());
  }
#if defined(__ATLTYPES_H__)
  inline BigRealPoint2D(const CPoint &p, DigitPool *digitPool=nullptr)
    : Point2DTemplate(BigReal(p.x,digitPool),BigReal(p.y,digitPool))
  {
  }
  inline BigRealPoint2D &operator=(const CPoint &p) {
    x() = p.x; y() = p.y;
    return *this;
  }
  inline explicit operator CPoint() const {
    return CPoint((int)round(x()), (int)round(y()));
  }
#endif // __ATLTYPES_H__
};

class BigRealIntervalTransformation {
private:
  const IntervalScale m_scaleType;
  BigRealInterval     m_fromInterval, m_toInterval;
  BigReal             m_a, m_b;
  UINT                m_precision, m_digits;
protected:
  virtual BigReal translate(       const BigReal &x) const = 0;
  virtual BigReal inverseTranslate(const BigReal &x) const = 0;
  void computeTransformation();
  void checkFromInterval(const TCHAR *method, const BigRealInterval &interval);
public:
  BigRealIntervalTransformation(const BigRealInterval &fromInterval, const BigRealInterval &toInterval, IntervalScale scaleType, UINT precision=AUTOPRECISION, DigitPool *digitPool = nullptr);
  // Set number of decimal digits in calculations
  // Specify AUTOPRECISION to get 8 extra decimal digits whatever from- and toInterval are
  inline IntervalScale getScaleType() const {
    return m_scaleType;
  }
  inline bool isLinear() const {
    return getScaleType() == LINEAR;
  }
  UINT setPrecision(UINT precsion);
  inline UINT getPrecision() const {
    return m_precision;
  }
  inline UINT getDigits() const {
    return m_digits;
  }
  inline DigitPool *getDigitPool() const {
    return m_fromInterval.getDigitPool();
  }
  static BigRealInterval getDefaultInterval(IntervalScale scale, DigitPool *digitPool=nullptr);

  inline const BigRealInterval &getFromInterval() const {
    return m_fromInterval;
  }
  inline const BigRealInterval &getToInterval() const {
    return m_toInterval;
  }
  virtual const BigRealInterval &setFromInterval(const BigRealInterval &interval);
  virtual const BigRealInterval &setToInterval(const BigRealInterval &interval);

  inline BigReal forwardTransform(const BigReal &x) const {
    DigitPool *dp = getDigitPool();
    return rProd(m_a,translate(x),m_digits, getDigitPool()) + m_b;
  }
  inline BigReal backwardTransform(const BigReal &x) const {
    DigitPool *dp = getDigitPool();
    return m_a.isZero() ? BigReal(getFromInterval().getFrom(),dp) : inverseTranslate(rQuot(dif(x,m_b,dp->_0(),dp),m_a,m_digits,dp));
  }
  inline BigRealInterval forwardTransform(const BigRealInterval &interval) const {
    return BigRealInterval(forwardTransform(interval.getFrom()),forwardTransform(interval.getTo()));
  }
  inline BigRealInterval backwardTransform(const BigRealInterval &interval) const {
    return BigRealInterval(backwardTransform(interval.getFrom()),backwardTransform(interval.getTo()));
  }

  // Returns new fromInterval.
  const BigRealInterval &zoom(const BigReal &x, const BigReal &factor, bool xInToInterval=true);
  virtual BigRealIntervalTransformation *clone(DigitPool *digitPool=nullptr) const = 0;
};

class BigRealLinearTransformation : public BigRealIntervalTransformation {
protected:
  BigReal translate(const BigReal &x) const override {
    return BigReal(x, getDigitPool());
  }
  BigReal inverseTranslate(const BigReal &x) const override {
    return BigReal(x, getDigitPool());
  }
public:
  BigRealLinearTransformation(const BigRealInterval &fromInterval, const BigRealInterval &toInterval, UINT precision=AUTOPRECISION, DigitPool *digitPool=nullptr)
    : BigRealIntervalTransformation(fromInterval, toInterval, LINEAR, precision, digitPool) {
    computeTransformation();
  }
  BigRealIntervalTransformation *clone(DigitPool *digitPool=nullptr) const override {
    return new BigRealLinearTransformation(getFromInterval(),getToInterval(),getPrecision(),digitPool);
  }
};


class BigRealRectangle2D : public Rectangle2DTemplate<BigRealPoint2D, BigRealSize2D, BigReal> {
public:
  inline BigRealRectangle2D(DigitPool *digitPool = nullptr)
    : Rectangle2DTemplate(BigReal(0,digitPool), BigReal(0,digitPool), BigReal(0, digitPool), BigReal(0, digitPool))
  {
  }
  inline BigRealRectangle2D(const BigReal &x, const BigReal &y, const BigReal &w, const BigReal &h, DigitPool *digitPool = nullptr)
    : Rectangle2DTemplate(BigReal(x,digitPool?digitPool:x.getDigitPool())
                         ,BigReal(y,digitPool?digitPool:x.getDigitPool())
                         ,BigReal(w,digitPool?digitPool:x.getDigitPool())
                         ,BigReal(h,digitPool?digitPool:x.getDigitPool()))
  {
  }
  inline BigRealRectangle2D(const BigRealPoint2D &topLeft, const BigRealPoint2D &bottomRight, DigitPool *digitPool = nullptr)
    : Rectangle2DTemplate(BigReal(topLeft.x()            ,digitPool?digitPool:topLeft.getDigitPool())
                         ,BigReal(topLeft.y()            ,digitPool?digitPool:topLeft.getDigitPool())
                         ,dif(bottomRight.x(),topLeft.x(),digitPool?digitPool:topLeft.getDigitPool())
                         ,dif(bottomRight.y(),topLeft.y(),digitPool?digitPool:topLeft.getDigitPool()))
  {
  }
  inline BigRealRectangle2D(const BigRealPoint2D &p, const BigRealSize2D &size, DigitPool *digitPool = nullptr)
    : Rectangle2DTemplate(BigRealPoint2D(p   ,digitPool)
                         ,BigRealSize2D( size,digitPool)
                         )
  {
  }
  inline DigitPool *getDigitPool() const {
    return ((BigRealPoint2D&)p0()).getDigitPool();
  }
  template<typename T> BigRealRectangle2D &operator=(const T &r) {
    DigitPool *dp = getDigitPool();
    *this = BigRealRectangle2D(BigReal(r.getX()     ,dp)
                              ,BigReal(r.getY()     ,dp)
                              ,BigReal(r.getWidth() ,dp)
                              ,BigReal(r.getHeight(),dp));
    return *this;
  }
  inline operator RealRectangle2D() const {
    return RealRectangle2D((Real)getX(), (Real)getY(), (Real)getWidth(), (Real)getHeight());
  }

  inline size_t getNeededDecimalDigits(size_t digits) const {
    const size_t digitsX = ((BigRealInterval&)getXInterval()).getNeededDecimalDigits(digits);
    const size_t digitsY = ((BigRealInterval&)getYInterval()).getNeededDecimalDigits(digits);
    return max(digitsX, digitsY);
  }
};

class BigRealRectangleTransformation {
private:
  DECLARECLASSNAME;
  DigitPool                     *m_digitPool;
  BigRealIntervalTransformation *m_xtransform, *m_ytransform;

  BigRealIntervalTransformation *allocateTransformation(const BigRealInterval &from, const BigRealInterval &to, IntervalScale scale) const;
  void cleanup();
  void computeTransformation(const BigRealRectangle2D &from, const BigRealRectangle2D &to, IntervalScale xScale, IntervalScale yScale);

  BigRealRectangleTransformation(const BigRealIntervalTransformation &tx, const BigRealIntervalTransformation &ty, DigitPool *digitPool = nullptr);

  static BigRealRectangle2D getDefaultFromRectangle(IntervalScale xScale, IntervalScale yScale, DigitPool *digitPool=nullptr);

  static inline BigRealRectangle2D getDefaultToRectangle(DigitPool *digitPool=nullptr) {
    BigRealRectangle2D result(digitPool);
    result = RealRectangle2D(0, 100, 100, -100);
    return result;
  }

public:
  inline BigRealRectangleTransformation(IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR, DigitPool *digitPool=nullptr)
    : m_digitPool(digitPool?digitPool:DEFAULT_DIGITPOOL)
  {
    m_xtransform = m_ytransform = nullptr;
    computeTransformation(getDefaultFromRectangle(xScale,yScale,getDigitPool()), getDefaultToRectangle(getDigitPool()), xScale, yScale);
  }
  inline BigRealRectangleTransformation(const BigRealRectangle2D &from, const BigRealRectangle2D &to, IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR, DigitPool *digitPool=nullptr)
    : m_digitPool(digitPool?digitPool:from.getDigitPool())
  {
    m_xtransform = m_ytransform = nullptr;
    computeTransformation(from, to, xScale, yScale);
  }
  inline BigRealRectangleTransformation(const BigRealRectangleTransformation &src, DigitPool *digitPool=nullptr)
    : m_digitPool(digitPool?digitPool:src.getDigitPool())
  {
    m_xtransform = src.getXTransformation().clone(getDigitPool());
    m_ytransform = src.getYTransformation().clone(getDigitPool());
  }
  inline BigRealRectangleTransformation &operator=(const BigRealRectangleTransformation &src) {
    cleanup();
    m_xtransform = src.getXTransformation().clone(getDigitPool());
    m_ytransform = src.getYTransformation().clone(getDigitPool());
    return *this;
  }
  virtual ~BigRealRectangleTransformation() {
    cleanup();
  }
  inline DigitPool *getDigitPool() const {
    return m_digitPool;
  }
  inline const BigRealIntervalTransformation &getXTransformation() const {
    return *m_xtransform;
  }
  inline const BigRealIntervalTransformation &getYTransformation() const {
    return *m_ytransform;
  }

  inline void setFromRectangle(const BigRealRectangle2D &rect) {
    computeTransformation(rect,getToRectangle(),getXTransformation().getScaleType(),getYTransformation().getScaleType());
  }
  inline void setToRectangle(const BigRealRectangle2D &rect) {
    computeTransformation(getFromRectangle(),rect,getXTransformation().getScaleType(),getYTransformation().getScaleType());
  }
  BigRealRectangle2D  getFromRectangle() const;
  BigRealRectangle2D  getToRectangle()   const;

  inline BigRealPoint2D      forwardTransform(const Point2DTemplate<BigReal> &p)  const {
    return forwardTransform(p[0], p[1]);
  }
  inline BigRealPoint2D      backwardTransform(const Point2DTemplate<BigReal> &p) const {
    return backwardTransform(p[0], p[1]);
  }
  inline BigRealPoint2D      forwardTransform(const BigReal &x, const BigReal &y) const {
    return BigRealPoint2D(getXTransformation().forwardTransform(x),getYTransformation().forwardTransform(y));
  }
  inline BigRealPoint2D      backwardTransform(const BigReal &x, const BigReal &y) const {
    return BigRealPoint2D(getXTransformation().backwardTransform(x),getYTransformation().backwardTransform(y));
  }
  inline BigRealRectangle2D  forwardTransform(const BigRealRectangle2D &rect)  const {
    return BigRealRectangle2D(forwardTransform(rect.LT()), forwardTransform(rect.RB()));
  }
  inline BigRealRectangle2D  backwardTransform( const BigRealRectangle2D &rect)  const {
    return BigRealRectangle2D(backwardTransform(rect.LT()), backwardTransform(rect.RB()));
  }

  void setScale(IntervalScale newScale, int flags);
  // Returns new fromRectangle.
  BigRealRectangle2D zoom(const BigRealPoint2D &p, const BigReal &factor, int flags = X_AXIS | Y_AXIS, bool pInToRectangle=true);

  // returns true if transformation is changed
  bool adjustAspectRatio();

  static inline BigRealRectangleTransformation getId(DigitPool *digitPool=nullptr) {
    if(digitPool == nullptr) digitPool = DEFAULT_DIGITPOOL;
    return BigRealRectangleTransformation(BigRealRectangle2D(0,0,1,1,digitPool)
                                         ,BigRealRectangle2D(0,0,1,1,digitPool));
  }
};
