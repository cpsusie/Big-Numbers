#pragma once

#include "Transformation.h"
#include "BigRealInterval.h"

#define AUTOPRECISION 0

class BigRealIntervalTransformation {
private:
  BigRealInterval m_fromInterval, m_toInterval;
  BigReal         m_a, m_b;
  UINT            m_precision, m_digits;
protected:
  virtual BigReal translate(       const BigReal &x) const = 0;
  virtual BigReal inverseTranslate(const BigReal &x) const = 0;
  void computeTransformation();
  void checkFromInterval(const BigRealInterval &interval);
public:
  BigRealIntervalTransformation(const BigRealInterval &fromInterval, const BigRealInterval &toInterval, UINT precision=AUTOPRECISION);
  // Set number of decimal digits in calculations
  // Specify AUTOPRECISION to get 8 extra decimal digits whatever from- and toInterval are
  UINT setPrecision(UINT precsion);
  inline UINT getPrecision() const {
    return m_precision;
  }
  inline UINT getDigits() const {
    return m_digits;
  }

  static BigRealInterval getDefaultInterval(IntervalScale scale);

  inline const BigRealInterval &getFromInterval() const {
    return m_fromInterval;
  }
  inline const BigRealInterval &getToInterval() const {
    return m_toInterval;
  }
  virtual const BigRealInterval &setFromInterval(const BigRealInterval &interval);
  virtual const BigRealInterval &setToInterval(const BigRealInterval &interval);

  inline BigReal forwardTransform(const BigReal &x) const {
    return rProd(m_a,translate(x),m_digits) + m_b;
  }
  inline BigReal backwardTransform(const BigReal &x) const {
    return m_a.isZero() ? getFromInterval().getFrom() : inverseTranslate(rQuot(x-m_b,m_a,m_digits));
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
  virtual BigRealIntervalTransformation *clone() const = 0;
};

class BigRealLinearTransformation : public BigRealIntervalTransformation {
protected:
  BigReal translate(const BigReal &x) const {
    return x;
  }
  BigReal inverseTranslate(const BigReal &x) const {
    return x;
  }
public:
  bool isLinear() const {
    return true;
  }
  BigRealLinearTransformation(const BigRealInterval &fromInterval, const BigRealInterval &toInterval, UINT precision=AUTOPRECISION)
    : BigRealIntervalTransformation(fromInterval, toInterval, precision) {
    computeTransformation();
  }
  IntervalScale getScale() const {
    return LINEAR;
  }
  BigRealIntervalTransformation *clone() const {
    return new BigRealLinearTransformation(*this);
  }
};

typedef Point2DTemplate<BigReal>     BigRealPoint2D;
typedef Rectangle2DTemplate<BigReal> BigRealRectangle2D;

class BigRealRectangleTransformation {
private:
  DECLARECLASSNAME;
  BigRealIntervalTransformation *m_xtransform, *m_ytransform;

  BigRealIntervalTransformation *allocateTransformation(const BigRealInterval &from, const BigRealInterval &to, IntervalScale scale) const;
  void cleanup();
  void computeTransformation(const BigRealRectangle2D &from, const BigRealRectangle2D &to, IntervalScale xScale, IntervalScale yScale);

  BigRealRectangleTransformation(const BigRealIntervalTransformation &tx, const BigRealIntervalTransformation &ty);

  static BigRealRectangle2D getDefaultFromRectangle(IntervalScale xScale, IntervalScale yScale);

  static inline BigRealRectangle2D getDefaultToRectangle() {
    return BigRealRectangle2D(0, 100, 100, -100);
  }

public:
  inline BigRealRectangleTransformation(IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR) {
    m_xtransform = m_ytransform = NULL;
    computeTransformation(getDefaultFromRectangle(xScale,yScale), getDefaultToRectangle(), xScale, yScale);
  }
  inline BigRealRectangleTransformation(const BigRealRectangleTransformation &src) {
    m_xtransform = src.getXTransformation().clone();
    m_ytransform = src.getYTransformation().clone();
  }
  inline BigRealRectangleTransformation &operator=(const BigRealRectangleTransformation &src) {
    cleanup();
    m_xtransform = src.getXTransformation().clone();
    m_ytransform = src.getYTransformation().clone();
    return *this;
  }
  inline BigRealRectangleTransformation(const BigRealRectangle2D &from, const BigRealRectangle2D &to, IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR) {
    m_xtransform = m_ytransform = NULL;
    computeTransformation(from, to, xScale, yScale);
  }
  virtual ~BigRealRectangleTransformation() {
    cleanup();
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

  inline BigRealPoint2D      forwardTransform(const BigRealPoint2D &p)   const {
    return BigRealPoint2D(getXTransformation().forwardTransform(p.x),getYTransformation().forwardTransform(p.y));
  }
  inline BigRealPoint2D      backwardTransform(const BigRealPoint2D &p)   const {
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

  static inline BigRealRectangleTransformation getId() {
    return BigRealRectangleTransformation(BigRealRectangle2D(0,0,1,1)
                                         ,BigRealRectangle2D(0,0,1,1));
  }
};
