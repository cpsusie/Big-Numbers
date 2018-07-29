#pragma once

#include "Transformation.h"
#include "BigRealInterval.h"

class BigRealIntervalTransformation {
private:
  DECLARECLASSNAME;
private:
  BigRealInterval m_fromInterval, m_toInterval;
  BigReal         m_a, m_b;
protected:
  UINT m_digits;
  void checkPrecision(UINT digits);
  virtual BigReal translate(       const BigReal &x) const = 0;
  virtual BigReal inverseTranslate(const BigReal &x) const = 0;
  void computeTransformation() {
    const BigRealInterval &toInterval   = getToInterval();
    const BigRealInterval &fromInterval = getFromInterval();
    m_a = rQuot(toInterval.getLength() , (translate(fromInterval.getTo()) - translate(fromInterval.getFrom())), m_digits);
    m_b = rDif(toInterval.getFrom(), rProd(m_a,translate(fromInterval.getFrom()), m_digits), m_digits);
  }
  void checkFromInterval(const BigRealInterval &interval) {
    if(interval.getLength().isZero()) {
      throwException(_T("IntervalTransformation::Invalid from-interval:%s"),interval.toString().cstr());
    }
  }
public:
  BigRealIntervalTransformation(const BigRealInterval &fromInterval, const BigRealInterval &toInterval, UINT digits) {
    setPrecision(digits);
    checkFromInterval(fromInterval);
    m_fromInterval = fromInterval;
    m_toInterval   = toInterval;
  }
  UINT setPrecision(UINT digits);
  inline UINT getPrecision() const {
    return m_digits;
  }

  static BigRealInterval getDefaultInterval(IntervalScale scale) {
    switch(scale) {
    case LINEAR             : return BigRealInterval(-10,10);
    default                 : throwInvalidArgumentException(__TFUNCTION__
                                                           ,_T("scale (=%d)"), scale);
                              return BigRealInterval(0,10);
    }
  }

  const BigRealInterval &getFromInterval() const {
    return m_fromInterval;
  }
  const BigRealInterval &getToInterval() const {
    return m_toInterval;
  }
  virtual const BigRealInterval &setFromInterval(const BigRealInterval &interval) {
    checkFromInterval(interval);
    m_fromInterval = interval;
    computeTransformation();
    return m_fromInterval;
  }
  virtual const BigRealInterval &setToInterval(const BigRealInterval &interval) {
    m_toInterval = interval;
    computeTransformation();
    return m_toInterval;
  }

  BigReal forwardTransform(const BigReal &x) const {
    return rSum(rProd(m_a,translate(x),m_digits), m_b, m_digits);
  }
  BigReal backwardTransform(const BigReal &x) const {
    return m_a.isZero() ? getFromInterval().getFrom() : inverseTranslate(rQuot(rDif(x,m_b,m_digits),m_a,m_digits));
  }
  BigRealInterval forwardTransform(const BigRealInterval &interval) const {
    return BigRealInterval(forwardTransform(interval.getFrom()),forwardTransform(interval.getTo()));
  }
  BigRealInterval backwardTransform(const BigRealInterval &interval) const {
    return BigRealInterval(backwardTransform(interval.getFrom()),backwardTransform(interval.getTo()));
  }

  virtual bool isLinear() const = 0;
  // Returns new fromInterval.
  const BigRealInterval &zoom(const BigReal &x, const BigReal &factor, bool xInToInterval=true) {
    if(xInToInterval) {
      if(!getToInterval().contains(x)) {
        return getFromInterval();
      }
    } else {
      if(!getFromInterval().contains(x)) {
        return getFromInterval();
      }
    }
    BigReal x1     = xInToInterval ? translate(backwardTransform(x)) : x;
    BigReal tFrom  = translate(getFromInterval().getFrom());
    BigReal tTo    = translate(getFromInterval().getTo());
    tFrom += rProd(rDif(x1, tFrom, m_digits), factor, m_digits);
    tTo   += rProd(rDif(x1, tTo  , m_digits), factor, m_digits);
    return setFromInterval(BigRealInterval(inverseTranslate(tFrom), inverseTranslate(tTo)));
  }

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
  BigRealLinearTransformation(const BigRealInterval &fromInterval, const BigRealInterval &toInterval, UINT digits)
    : BigRealIntervalTransformation(fromInterval, toInterval, digits) {
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
  UINT                           m_digits;

  void checkPrecision(UINT digits);
  void cleanup() {
    SAFEDELETE(m_xtransform);
    SAFEDELETE(m_ytransform);
  }
  void computeTransformation(const BigRealRectangle2D &from, const BigRealRectangle2D &to, IntervalScale xScale, IntervalScale yScale) {
    BigRealIntervalTransformation *newXtransform = NULL, *newYtransform = NULL;
    try {
      newXtransform = allocateTransformation(BigRealInterval(from.getBottomLeft().x,from.getBottomRight().x)
                                            ,BigRealInterval(to.getBottomLeft().x  ,to.getBottomRight().x)
                                            ,xScale);

      newYtransform = allocateTransformation(BigRealInterval(from.getTopLeft().y, from.getBottomLeft().y)
                                            ,BigRealInterval(to.getTopLeft().y  , to.getBottomLeft().y  )
                                            ,yScale);
      cleanup();
      m_xtransform = newXtransform;
      m_ytransform = newYtransform;
    } catch(Exception e) {
      SAFEDELETE(newXtransform);
      SAFEDELETE(newYtransform);
      throw;
    }
  }

  BigRealRectangleTransformation(const BigRealIntervalTransformation &tx, const BigRealIntervalTransformation &ty) {
    setPrecision(dmax(tx.getPrecision(),ty.getPrecision()));
    m_xtransform = tx.clone();
    m_ytransform = ty.clone();
  }

  static BigRealRectangle2D getDefaultFromRectangle(IntervalScale xScale, IntervalScale yScale) {
    BigRealInterval xInterval = BigRealIntervalTransformation::getDefaultInterval(xScale);
    BigRealInterval yInterval = BigRealIntervalTransformation::getDefaultInterval(yScale);
    return BigRealRectangle2D(xInterval.getFrom(), yInterval.getFrom(), xInterval.getLength(), yInterval.getLength());
  }
  static BigRealRectangle2D getDefaultToRectangle() {
    return BigRealRectangle2D(0, 100, 100, -100);
  }
protected:
  BigRealIntervalTransformation *allocateTransformation(const BigRealInterval &from, const BigRealInterval &to, IntervalScale scale) const {
    switch(scale) {
    case LINEAR             : return new BigRealLinearTransformation(from, to, m_digits);
    default                 : throwInvalidArgumentException(__TFUNCTION__,_T("scale=%d"), scale);
                              return NULL;
    }
  }

public:
  BigRealRectangleTransformation(UINT digits, IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR) {
    setPrecision(digits);
    m_xtransform = m_ytransform = NULL;
    computeTransformation(getDefaultFromRectangle(xScale,yScale), getDefaultToRectangle(), xScale, yScale);
  }
  BigRealRectangleTransformation(const BigRealRectangleTransformation &src) {
    setPrecision(src.getPrecision());
    m_xtransform = src.getXTransformation().clone();
    m_ytransform = src.getYTransformation().clone();
  }
  BigRealRectangleTransformation &operator=(const BigRealRectangleTransformation &src) {
    cleanup();
    setPrecision(src.getPrecision());
    m_xtransform = src.getXTransformation().clone();
    m_ytransform = src.getYTransformation().clone();
    return *this;
  }
  BigRealRectangleTransformation(UINT digits, const BigRealRectangle2D &from, const BigRealRectangle2D &to, IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR) {
    setPrecision(digits);
    m_xtransform = m_ytransform = NULL;
    computeTransformation(from, to, xScale, yScale);
  }
  UINT setPrecision(UINT digits);
  inline UINT getPrecision() const {
    return m_digits;
  }
  virtual ~BigRealRectangleTransformation() {
    cleanup();
  }
  const BigRealIntervalTransformation &getXTransformation() const {
    return *m_xtransform;
  }
  const BigRealIntervalTransformation &getYTransformation() const {
    return *m_ytransform;
  }

  void   setFromRectangle(const BigRealRectangle2D &rect) {
    computeTransformation(rect,getToRectangle(),getXTransformation().getScale(),getYTransformation().getScale());
  }
  void   setToRectangle(const BigRealRectangle2D &rect) {
    computeTransformation(getFromRectangle(),rect,getXTransformation().getScale(),getYTransformation().getScale());
  }
  BigRealRectangle2D  getFromRectangle() const {
    BigRealPoint2D p1 = BigRealPoint2D(getXTransformation().getFromInterval().getFrom(), getYTransformation().getFromInterval().getFrom());
    BigRealPoint2D p2 = BigRealPoint2D(getXTransformation().getFromInterval().getTo()  , getYTransformation().getFromInterval().getTo());
    return BigRealRectangle2D(p1.x, p1.y, p2.x-p1.x, p2.y-p1.y);
  }
  BigRealRectangle2D  getToRectangle()   const {
    BigRealPoint2D p1 = BigRealPoint2D(getXTransformation().getToInterval().getFrom()  , getYTransformation().getToInterval().getFrom());
    BigRealPoint2D p2 = BigRealPoint2D(getXTransformation().getToInterval().getTo()    , getYTransformation().getToInterval().getTo());
    return BigRealRectangle2D(p1.x, p1.y, p2.x-p1.x, p2.y-p1.y);
  }
  BigRealPoint2D      forwardTransform(const BigRealPoint2D &p)   const {
    return BigRealPoint2D(getXTransformation().forwardTransform(p.x),getYTransformation().forwardTransform(p.y));
  }
  BigRealPoint2D      backwardTransform(const BigRealPoint2D &p)   const {
    return BigRealPoint2D(getXTransformation().backwardTransform(p.x),getYTransformation().backwardTransform(p.y));
  }
  BigRealPoint2D      forwardTransform(const BigReal &x, const BigReal &y) const {
    return BigRealPoint2D(getXTransformation().forwardTransform(x),getYTransformation().forwardTransform(y));
  }
  BigRealPoint2D      backwardTransform(const BigReal &x, const BigReal &y) const {
    return BigRealPoint2D(getXTransformation().backwardTransform(x),getYTransformation().backwardTransform(y));
  }
  BigRealRectangle2D  forwardTransform(const BigRealRectangle2D &rect)  const {
    BigRealPoint2D p1 = forwardTransform(rect.getTopLeft());
    BigRealPoint2D p2 = forwardTransform(rect.getBottomRight());
    return BigRealRectangle2D(p1.x,p1.y,p2.x-p1.x,p2.y-p1.y);
  }
  BigRealRectangle2D  backwardTransform( const BigRealRectangle2D &rect)  const {
    BigRealPoint2D p1 = backwardTransform(rect.getTopLeft());
    BigRealPoint2D p2 = backwardTransform(rect.getBottomRight());
    return BigRealRectangle2D(p1.x,p1.y,p2.x-p1.x,p2.y-p1.y);
  }

  void setScale(IntervalScale newScale, int flags) {
    IntervalScale xScale = getXTransformation().getScale();
    IntervalScale yScale = getYTransformation().getScale();
    if((flags & X_AXIS) != 0) {
      xScale = newScale;
    }
    if((flags & Y_AXIS) != 0) {
      yScale = newScale;
    }
    computeTransformation(getFromRectangle(),getToRectangle(),xScale,yScale);
  }
  // Returns new fromRectangle.
  BigRealRectangle2D zoom(const BigRealPoint2D &p, const BigReal &factor, int flags = X_AXIS | Y_AXIS, bool pInToRectangle=true) {
    if(flags & X_AXIS) {
      m_xtransform->zoom(p.x, factor, pInToRectangle);
    }
    if(flags & Y_AXIS) {
      m_ytransform->zoom(p.y, factor, pInToRectangle);
    }
    return getFromRectangle();
  }
  // returns true if transformation is changed
  virtual bool adjustAspectRatio() {
    if(!getXTransformation().isLinear() || !getYTransformation().isLinear()) {
      return false;
    }
    BigRealRectangle2D fr        = getFromRectangle();
    BigRealRectangle2D tr        = getToRectangle();
    const BigReal      fromRatio = fabs(rQuot(fr.getWidth() , fr.getHeight(), m_digits));
    const BigReal      toRatio   = fabs(rQuot(tr.getWidth() , tr.getHeight(), m_digits));
    bool               changed   = false;
    if(fromRatio > toRatio) {
      const BigReal dh = dsign(fr.getHeight())*rDif(fabs(rQuot(fr.getWidth(),toRatio,m_digits)), fabs(fr.getHeight()),m_digits);
      fr.m_y -= dh * dh.getDigitPool()->getHalf();
      fr.m_h += dh;
      changed = !dh.isZero();
    } else if(fromRatio < toRatio) {
      const BigReal dw = dsign(fr.getWidth())*rDif(fabs(rProd(toRatio,fr.getHeight(),m_digits)),fabs(fr.getWidth()), m_digits);
      fr.m_x -= dw * dw.getDigitPool()->getHalf();
      fr.m_w += dw;
      changed = !dw.isZero();
    }
    if(changed) {
      setFromRectangle(fr);
    }
    return changed;
  }

  static BigRealRectangleTransformation getId() {
    return BigRealRectangleTransformation(15,BigRealRectangle2D(0,0,1,1)
                                            ,BigRealRectangle2D(0,0,1,1));
  }
};

