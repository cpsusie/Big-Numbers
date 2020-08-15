#pragma once

#include <Math/Transformation.h>
#include "BigRealInterval.h"

#define AUTOPRECISION 0

class BigRealSize2D : public Size2DTemplate<BigReal> {
  inline BigRealSize2D(DigitPool *digitPool=NULL)
    : Size2DTemplate(BigReal(0,digitPool),BigReal(0,digitPool))
  {
  }
  inline BigRealSize2D(const BigReal &cx, const BigReal &cy, DigitPool *digitPool=NULL)
    : Size2DTemplate(BigReal(cx,digitPool?digitPool:cx.getDigitPool())
                    ,BigReal(cy,digitPool?digitPool:cx.getDigitPool()))
  {
  }
  inline BigRealSize2D(const BigRealSize2D &src, DigitPool *digitPool=NULL)
    : Size2DTemplate(BigReal(src.cx,digitPool?digitPool:src.getDigitPool())
                    ,BigReal(src.cy,digitPool?digitPool:src.getDigitPool()))
  {
  }
  inline DigitPool *getDigitPool() const {
    return cx.getDigitPool();
  }
  template<typename T> BigRealSize2D &operator=(const T &src) {
    DigitPool *dp = getDigitPool();
    x = BigReal(src.cx,dp);
    y = BigReal(src.cy,dp);
    return *this;
  }
  inline operator RealSize2D() const {
    return RealSize2D((Real)cx, (Real)cy);
  }
};

class BigRealPoint2D : public Point2DTemplate<BigReal> {
public:
  inline BigRealPoint2D(DigitPool *digitPool=NULL)
    : Point2DTemplate(BigReal(0,digitPool)
                     ,BigReal(0,digitPool))
  {
  }
  inline BigRealPoint2D(const BigReal &x, const BigReal &y, DigitPool *digitPool=NULL)
    : Point2DTemplate(BigReal(x,digitPool?digitPool:x.getDigitPool())
                     ,BigReal(y,digitPool?digitPool:x.getDigitPool()))
  {
  }
  inline BigRealPoint2D(const BigRealPoint2D &src, DigitPool *digitPool=NULL)
    : Point2DTemplate(BigReal(src.x,digitPool?digitPool:src.getDigitPool())
                     ,BigReal(src.y,digitPool?digitPool:src.getDigitPool()))
  {
  }
  inline DigitPool *getDigitPool() const {
    return x.getDigitPool();
  }
  template<typename T> BigRealPoint2D &operator=(const T &src) {
    DigitPool *dp = getDigitPool();
    x = BigReal(src.x,dp);
    y = BigReal(src.y,dp);
    return *this;
  }
  inline operator RealPoint2D() const {
    return RealPoint2D((Real)x, (Real)y);
  }
};

class BigRealIntervalTransformation {
private:
  BigRealInterval m_fromInterval, m_toInterval;
  BigReal         m_a, m_b;
  UINT            m_precision, m_digits;
protected:
  virtual BigReal translate(       const BigReal &x) const = 0;
  virtual BigReal inverseTranslate(const BigReal &x) const = 0;
  void computeTransformation();
  void checkFromInterval(const TCHAR *method, const BigRealInterval &interval);
public:
  BigRealIntervalTransformation(const BigRealInterval &fromInterval, const BigRealInterval &toInterval, UINT precision=AUTOPRECISION, DigitPool *digitPool = NULL);
  // Set number of decimal digits in calculations
  // Specify AUTOPRECISION to get 8 extra decimal digits whatever from- and toInterval are
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
  static BigRealInterval getDefaultInterval(IntervalScale scale, DigitPool *digitPool=NULL);

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

  virtual bool isLinear() const = 0;
  // Returns new fromInterval.
  const BigRealInterval &zoom(const BigReal &x, const BigReal &factor, bool xInToInterval=true);
  virtual IntervalScale getScale() const = 0;
  virtual BigRealIntervalTransformation *clone(DigitPool *digitPool=NULL) const = 0;
};

class BigRealLinearTransformation : public BigRealIntervalTransformation {
protected:
  BigReal translate(const BigReal &x) const {
    return BigReal(x, getDigitPool());
  }
  BigReal inverseTranslate(const BigReal &x) const {
    return BigReal(x, getDigitPool());
  }
public:
  bool isLinear() const {
    return true;
  }
  BigRealLinearTransformation(const BigRealInterval &fromInterval, const BigRealInterval &toInterval, UINT precision=AUTOPRECISION, DigitPool *digitPool=NULL)
    : BigRealIntervalTransformation(fromInterval, toInterval, precision, digitPool) {
    computeTransformation();
  }
  IntervalScale getScale() const {
    return LINEAR;
  }
  BigRealIntervalTransformation *clone(DigitPool *digitPool=NULL) const {
    return new BigRealLinearTransformation(getFromInterval(),getToInterval(),getPrecision(),digitPool);
  }
};


class BigRealRectangle2D : public Rectangle2DTemplate<BigReal> {
public:
  inline BigRealRectangle2D(DigitPool *digitPool = NULL)
    : Rectangle2DTemplate(BigReal(0,digitPool), BigReal(0,digitPool), BigReal(0, digitPool), BigReal(0, digitPool))
  {
  }
  inline BigRealRectangle2D(const BigReal &x, const BigReal &y, const BigReal &w, const BigReal &h, DigitPool *digitPool = NULL)
    : Rectangle2DTemplate(BigReal(x,digitPool?digitPool:x.getDigitPool())
                         ,BigReal(y,digitPool?digitPool:x.getDigitPool())
                         ,BigReal(w,digitPool?digitPool:x.getDigitPool())
                         ,BigReal(h,digitPool?digitPool:x.getDigitPool()))
  {
  }
  inline BigRealRectangle2D(const BigRealPoint2D &topLeft, const BigRealPoint2D &bottomRight, DigitPool *digitPool = NULL)
    : Rectangle2DTemplate(BigReal(topLeft.x          ,digitPool?digitPool:topLeft.getDigitPool())
                         ,BigReal(topLeft.y          ,digitPool?digitPool:topLeft.getDigitPool())
                         ,dif(bottomRight.x,topLeft.x,digitPool?digitPool:topLeft.getDigitPool())
                         ,dif(bottomRight.y,topLeft.y,digitPool?digitPool:topLeft.getDigitPool()))
  {
  }
  inline BigRealRectangle2D(const BigRealPoint2D &p, const BigRealSize2D &size, DigitPool *digitPool = NULL)
    : Rectangle2DTemplate(BigReal(p.x    ,digitPool)
                         ,BigReal(p.y    ,digitPool)
                         ,BigReal(size.cx,digitPool)
                         ,BigReal(size.cy,digitPool))
  {
  }
  inline DigitPool *getDigitPool() const {
    return m_x.getDigitPool();
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

  BigRealRectangleTransformation(const BigRealIntervalTransformation &tx, const BigRealIntervalTransformation &ty, DigitPool *digitPool = NULL);

  static BigRealRectangle2D getDefaultFromRectangle(IntervalScale xScale, IntervalScale yScale, DigitPool *digitPool=NULL);

  static inline BigRealRectangle2D getDefaultToRectangle(DigitPool *digitPool=NULL) {
    BigRealRectangle2D result(digitPool);
    result = RealRectangle2D(0, 100, 100, -100);
    return result;
  }

public:
  inline BigRealRectangleTransformation(IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR, DigitPool *digitPool=NULL)
    : m_digitPool(digitPool?digitPool:DEFAULT_DIGITPOOL)
  {
    m_xtransform = m_ytransform = NULL;
    computeTransformation(getDefaultFromRectangle(xScale,yScale,getDigitPool()), getDefaultToRectangle(getDigitPool()), xScale, yScale);
  }
  inline BigRealRectangleTransformation(const BigRealRectangle2D &from, const BigRealRectangle2D &to, IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR, DigitPool *digitPool=NULL)
    : m_digitPool(digitPool?digitPool:from.getDigitPool())
  {
    m_xtransform = m_ytransform = NULL;
    computeTransformation(from, to, xScale, yScale);
  }
  inline BigRealRectangleTransformation(const BigRealRectangleTransformation &src, DigitPool *digitPool=NULL)
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
    computeTransformation(rect,getToRectangle(),getXTransformation().getScale(),getYTransformation().getScale());
  }
  inline void setToRectangle(const BigRealRectangle2D &rect) {
    computeTransformation(getFromRectangle(),rect,getXTransformation().getScale(),getYTransformation().getScale());
  }
  BigRealRectangle2D  getFromRectangle() const;
  BigRealRectangle2D  getToRectangle()   const;

  inline BigRealPoint2D      forwardTransform(const Point2DTemplate<BigReal> &p)   const {
    return BigRealPoint2D(getXTransformation().forwardTransform(p.x),getYTransformation().forwardTransform(p.y));
  }
  inline BigRealPoint2D      backwardTransform(const Point2DTemplate<BigReal> &p)   const {
    return BigRealPoint2D(getXTransformation().backwardTransform(p.x),getYTransformation().backwardTransform(p.y));
  }
  inline BigRealPoint2D      forwardTransform(const BigReal &x, const BigReal &y) const {
    return BigRealPoint2D(getXTransformation().forwardTransform(x),getYTransformation().forwardTransform(y));
  }
  inline BigRealPoint2D      backwardTransform(const BigReal &x, const BigReal &y) const {
    return BigRealPoint2D(getXTransformation().backwardTransform(x),getYTransformation().backwardTransform(y));
  }
  inline BigRealRectangle2D  forwardTransform(const BigRealRectangle2D &rect)  const {
    const BigRealPoint2D p1 = forwardTransform(rect.getTopLeft());
    const BigRealPoint2D p2 = forwardTransform(rect.getBottomRight());
    return BigRealRectangle2D(p1.x,p1.y,p2.x-p1.x,p2.y-p1.y);
  }
  inline BigRealRectangle2D  backwardTransform( const BigRealRectangle2D &rect)  const {
    const BigRealPoint2D p1 = backwardTransform(rect.getTopLeft());
    const BigRealPoint2D p2 = backwardTransform(rect.getBottomRight());
    return BigRealRectangle2D(p1.x,p1.y,p2.x-p1.x,p2.y-p1.y);
  }

  void setScale(IntervalScale newScale, int flags);
  // Returns new fromRectangle.
  BigRealRectangle2D zoom(const BigRealPoint2D &p, const BigReal &factor, int flags = X_AXIS | Y_AXIS, bool pInToRectangle=true);

  // returns true if transformation is changed
  bool adjustAspectRatio();

  static inline BigRealRectangleTransformation getId(DigitPool *digitPool=NULL) {
    if(digitPool == NULL) digitPool = DEFAULT_DIGITPOOL;
    return BigRealRectangleTransformation(BigRealRectangle2D(0,0,1,1,digitPool)
                                         ,BigRealRectangle2D(0,0,1,1,digitPool));
  }
};
