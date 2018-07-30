#include "pch.h"
#include <Math/BigRealTransformation.h>

UINT BigRealIntervalTransformation::setPrecision(UINT precision) {
  const UINT oldPrecision = m_precision;
  m_precision = m_digits = precision;
  return oldPrecision;
}

void BigRealIntervalTransformation::computeTransformation() {
  const BigRealInterval &toInterval   = getToInterval();
  const BigRealInterval &fromInterval = getFromInterval();
  const BigReal transFromTo           = translate(fromInterval.getTo()), transFromFrom = translate(fromInterval.getFrom());
  const BigReal transFromLength       = transFromTo - transFromFrom;
  const BigReal toLength              = toInterval.getLength();
  if(m_precision == AUTOPRECISION) {
    const BRExpoType fromLengthE10 = BigReal::getExpo10(transFromLength);
    const BRExpoType toLengthE10   = BigReal::getExpo10(toLength);
    const BRExpoType aE10          = toLengthE10 - fromLengthE10;
    const BRExpoType fromFromE10   = BigReal::getExpo10(transFromFrom);
    const BRExpoType toFromE10     = BigReal::getExpo10(toInterval.getFrom());
    // aE10 + fromFromE10 - m_digits = toFromE10 - 8
    m_digits = aE10 + fromFromE10 - toFromE10 + 8;
  }
  m_a = rQuot(toLength, transFromLength, m_digits);
  m_b = toInterval.getFrom() - rProd(m_a,transFromFrom, m_digits);
}

void BigRealIntervalTransformation::checkFromInterval(const BigRealInterval &interval) {
  if(interval.getLength().isZero()) {
    throwException(_T("IntervalTransformation::Invalid from-interval:%s"),interval.toString().cstr());
  }
}

BigRealIntervalTransformation::BigRealIntervalTransformation(const BigRealInterval &fromInterval, const BigRealInterval &toInterval, UINT precision) {
  setPrecision(precision);
  checkFromInterval(fromInterval);
  m_fromInterval = fromInterval;
  m_toInterval   = toInterval;
}

BigRealInterval BigRealIntervalTransformation::getDefaultInterval(IntervalScale scale) { // static
  switch(scale) {
  case LINEAR             : return BigRealInterval(-10,10);
  default                 : throwInvalidArgumentException(__TFUNCTION__
                                                          ,_T("scale (=%d)"), scale);
                            return BigRealInterval(0,10);
  }
}

const BigRealInterval &BigRealIntervalTransformation::setFromInterval(const BigRealInterval &interval) {
  checkFromInterval(interval);
  m_fromInterval = interval;
  computeTransformation();
  return m_fromInterval;
}

const BigRealInterval &BigRealIntervalTransformation::setToInterval(const BigRealInterval &interval) {
  m_toInterval = interval;
  computeTransformation();
  return m_toInterval;
}

// Returns new fromInterval.
const BigRealInterval &BigRealIntervalTransformation::zoom(const BigReal &x, const BigReal &factor, bool xInToInterval) {
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
  tFrom += rProd(x1 - tFrom, factor, m_digits);
  tTo   += rProd(x1 - tTo  , factor, m_digits);
  return setFromInterval(BigRealInterval(inverseTranslate(tFrom), inverseTranslate(tTo)));
}

DEFINECLASSNAME(BigRealRectangleTransformation);

BigRealIntervalTransformation *BigRealRectangleTransformation::allocateTransformation(const BigRealInterval &from, const BigRealInterval &to, IntervalScale scale) const {
  switch(scale) {
  case LINEAR             : return new BigRealLinearTransformation(from, to);
  default                 : throwInvalidArgumentException(__TFUNCTION__,_T("scale=%d"), scale);
                            return NULL;
  }
}

void BigRealRectangleTransformation::cleanup() {
  SAFEDELETE(m_xtransform);
  SAFEDELETE(m_ytransform);
}

void BigRealRectangleTransformation::computeTransformation(const BigRealRectangle2D &from, const BigRealRectangle2D &to, IntervalScale xScale, IntervalScale yScale) {
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

BigRealRectangleTransformation::BigRealRectangleTransformation(const BigRealIntervalTransformation &tx, const BigRealIntervalTransformation &ty) {
  m_xtransform = tx.clone();
  m_ytransform = ty.clone();
  m_xtransform->setPrecision(AUTOPRECISION);
  m_ytransform->setPrecision(AUTOPRECISION);
}

BigRealRectangle2D BigRealRectangleTransformation::getDefaultFromRectangle(IntervalScale xScale, IntervalScale yScale) { // static
  BigRealInterval xInterval = BigRealIntervalTransformation::getDefaultInterval(xScale);
  BigRealInterval yInterval = BigRealIntervalTransformation::getDefaultInterval(yScale);
  return BigRealRectangle2D(xInterval.getFrom(), yInterval.getFrom(), xInterval.getLength(), yInterval.getLength());
}

BigRealRectangle2D  BigRealRectangleTransformation::getFromRectangle() const {
  BigRealPoint2D p1 = BigRealPoint2D(getXTransformation().getFromInterval().getFrom(), getYTransformation().getFromInterval().getFrom());
  BigRealPoint2D p2 = BigRealPoint2D(getXTransformation().getFromInterval().getTo()  , getYTransformation().getFromInterval().getTo());
  return BigRealRectangle2D(p1.x, p1.y, p2.x-p1.x, p2.y-p1.y);
}

BigRealRectangle2D  BigRealRectangleTransformation::getToRectangle()   const {
  BigRealPoint2D p1 = BigRealPoint2D(getXTransformation().getToInterval().getFrom()  , getYTransformation().getToInterval().getFrom());
  BigRealPoint2D p2 = BigRealPoint2D(getXTransformation().getToInterval().getTo()    , getYTransformation().getToInterval().getTo());
  return BigRealRectangle2D(p1.x, p1.y, p2.x-p1.x, p2.y-p1.y);
}

void BigRealRectangleTransformation::setScale(IntervalScale newScale, int flags) {
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
BigRealRectangle2D BigRealRectangleTransformation::zoom(const BigRealPoint2D &p, const BigReal &factor, int flags, bool pInToRectangle) {
  if(flags & X_AXIS) {
    m_xtransform->zoom(p.x, factor, pInToRectangle);
  }
  if(flags & Y_AXIS) {
    m_ytransform->zoom(p.y, factor, pInToRectangle);
  }
  return getFromRectangle();
}

// returns true if transformation is changed
bool BigRealRectangleTransformation::adjustAspectRatio() {
  if(!getXTransformation().isLinear() || !getYTransformation().isLinear()) {
    return false;
  }

  BigRealRectangle2D fr        = getFromRectangle();
  BigRealRectangle2D tr        = getToRectangle();
  const BigReal      fromRatio = fabs(rQuot(fr.getWidth() , fr.getHeight(), 15));
  const BigReal      toRatio   = fabs(rQuot(tr.getWidth() , tr.getHeight(), 15));
  bool               changed   = false;
  const UINT         digits    = max(getXTransformation().getDigits(), getYTransformation().getDigits());
  if(fromRatio > toRatio) {
    const BigReal dh = dsign(fr.getHeight())*(fabs(rQuot(fr.getWidth(),toRatio,digits)) - fabs(fr.getHeight()));
    fr.m_y -= dh * dh.getDigitPool()->getHalf();
    fr.m_h += dh;
    changed = !dh.isZero();
  } else if(fromRatio < toRatio) {
    const BigReal dw = dsign(fr.getWidth())*(fabs(rProd(toRatio,fr.getHeight(),digits)),fabs(fr.getWidth()));
    fr.m_x -= dw * dw.getDigitPool()->getHalf();
    fr.m_w += dw;
    changed = !dw.isZero();
  }
  if(changed) {
    setFromRectangle(fr);
  }
  return changed;
}
