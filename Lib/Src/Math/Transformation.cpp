#include "pch.h"
#include <Math/Transformation.h>

// ----------------------------------- IntervalTransformation -----------------------------------

IntervalTransformation::IntervalTransformation(const DoubleInterval &fromInterval, const DoubleInterval &toInterval) {
  checkFromInterval(fromInterval);
  m_fromInterval = fromInterval;
  m_toInterval   = toInterval;
}

DoubleInterval IntervalTransformation::getDefaultInterval(IntervalScale scale) { // static
  switch(scale) {
  case LINEAR             : return DoubleInterval(-10,10);
  case LOGARITHMIC        : return DoubleInterval(1,10);
  case NORMAL_DISTRIBUTION: return DoubleInterval(1e-6,1-1e-6);
  default                 : throwException(_T("IntervalTransformation::getDefaultInterval:Invalid scale (=%d)."),scale);
                            return DoubleInterval(0,10);
  }
}

const DoubleInterval &IntervalTransformation::setFromInterval(const DoubleInterval &interval) {
  checkFromInterval(interval);
  m_fromInterval = interval;
  computeTransformation();
  return m_fromInterval;
}

void IntervalTransformation::checkFromInterval(const DoubleInterval &interval) {
  if(interval.getLength() == 0) {
    throwException(_T("IntervalTransformation::Invalid from-interval [%le;%le]"),m_fromInterval.getFrom(), m_fromInterval.getTo());
  }
}

const DoubleInterval &IntervalTransformation::setToInterval(const DoubleInterval &interval) {
  m_toInterval = interval;
  computeTransformation();
  return m_toInterval;
}

DoubleInterval IntervalTransformation::forwardTransform( const DoubleInterval &interval) const {
  return DoubleInterval(forwardTransform(interval.getFrom()),forwardTransform(interval.getTo()));
}

DoubleInterval IntervalTransformation::backwardTransform(const DoubleInterval &interval) const {
  return DoubleInterval(backwardTransform(interval.getFrom()),backwardTransform(interval.getTo()));
}

void IntervalTransformation::computeTransformation() {
  DoubleInterval toInterval   = getToInterval();
  DoubleInterval fromInterval = getFromInterval();
  m_a = toInterval.getLength() / (translate(fromInterval.getTo()) - translate(fromInterval.getFrom()));
  m_b = toInterval.getFrom() - m_a * translate(fromInterval.getFrom());
}

double IntervalTransformation::forwardTransform(double x) const {
  return m_a * translate(x) + m_b;
}

double IntervalTransformation::backwardTransform(double x) const {
  if(m_a == 0) {
    return getFromInterval().getFrom();
  } else {
    return inverseTranslate((x - m_b) / m_a);
  }
}

const DoubleInterval &IntervalTransformation::zoom(double x, double factor, bool xInToInterval) {
  if(xInToInterval) {
    if(!getToInterval().contains(x)) {
      return m_fromInterval;
    }
  } else {
    if(!getFromInterval().contains(x)) {
      return m_fromInterval;
    }
  }
  double x1     = xInToInterval ? translate(backwardTransform(x)) : x;
  double tFrom  = translate(getFromInterval().getFrom());
  double tTo    = translate(getFromInterval().getTo());
  tFrom += (x1 - tFrom) * factor;
  tTo   += (x1 - tTo  ) * factor;
  return setFromInterval(DoubleInterval(inverseTranslate(tFrom), inverseTranslate(tTo)));
}

// ----------------------------------- LinearTransformation -----------------------------------

LinearTransformation::LinearTransformation(const DoubleInterval &fromInterval, const DoubleInterval &toInterval)
 : IntervalTransformation(fromInterval, toInterval) {
  computeTransformation();
}

IntervalScale LinearTransformation::getScale() const {
  return LINEAR;
}

double LinearTransformation::translate(double x) const {
  return x;
}

double LinearTransformation::inverseTranslate(double x) const {
  return x;
}

IntervalTransformation *LinearTransformation::clone() const {
  return new LinearTransformation(*this);
}

// ----------------------------------- LogarithmicTransformation -----------------------------------

LogarithmicTransformation::LogarithmicTransformation(const DoubleInterval &fromInterval, const DoubleInterval &toInterval)
 : IntervalTransformation(fromInterval, toInterval) {
  computeTransformation();
}

IntervalScale LogarithmicTransformation::getScale() const {
  return LOGARITHMIC;
}

double LogarithmicTransformation::translate(double x) const {
  if(x <= 0) {
    throwException(_T("Negative number not allowed in logarithmic scale"));
  }
  return log(x);
}

double LogarithmicTransformation::inverseTranslate(double x) const {
  return exp(x);
}

IntervalTransformation *LogarithmicTransformation::clone() const {
  return new LogarithmicTransformation(*this);
}

// ----------------------------------- NormalDistributionTransformation -----------------------------------


DoubleInterval NormalDistributionTransformation::maxInterval(1e-12,1 - 1e-12);

NormalDistributionTransformation::NormalDistributionTransformation(const DoubleInterval &fromInterval, const DoubleInterval &toInterval)
 : IntervalTransformation(fromInterval, toInterval) {
  computeTransformation();
}

IntervalScale NormalDistributionTransformation::getScale() const {
  return NORMAL_DISTRIBUTION;
}

const DoubleInterval &NormalDistributionTransformation::setFromInterval(const DoubleInterval &i) {
  DoubleInterval n = i.interSection(maxInterval);
  if(n.getLength() == 0) {
    n = maxInterval;
  }
  return IntervalTransformation::setFromInterval(n);
}

double NormalDistributionTransformation::translate(double x) const {
  if(x <= 0 || x >= 1) {
    throwException(_T("NormalDistributionTransformation::translate:x (=%le) outside valid interval ]0;1["),x);
  }
  return getDouble(probitFunction(x));
}

double NormalDistributionTransformation::inverseTranslate(double x) const {
  return getDouble(norm(x));
}

IntervalTransformation *NormalDistributionTransformation::clone() const {
  return new NormalDistributionTransformation(*this);
}

// ----------------------------------- RectangleTransformation -----------------------------------

RectangleTransformation::RectangleTransformation(IntervalScale xScale, IntervalScale yScale ) {
  m_xtransform = m_ytransform = NULL;
  computeTransformation(getDefaultFromRectangle(xScale,yScale), getDefaultToRectangle(), xScale, yScale);
}

RectangleTransformation::RectangleTransformation(const Rectangle2D &from, const Rectangle2D &to, IntervalScale xScale, IntervalScale yScale) {
  m_xtransform = m_ytransform = NULL;
  computeTransformation(from, to, xScale, yScale);
}

RectangleTransformation::RectangleTransformation(const RectangleTransformation &src) {
  m_xtransform = src.getXTransformation().clone();
  m_ytransform = src.getYTransformation().clone();
}

RectangleTransformation &RectangleTransformation::operator=(const RectangleTransformation &src) {
  delete m_xtransform;
  delete m_ytransform;
  m_xtransform = src.getXTransformation().clone();
  m_ytransform = src.getYTransformation().clone();
  return *this;
}

RectangleTransformation::RectangleTransformation(const IntervalTransformation &tx, const IntervalTransformation &ty) {
  m_xtransform = tx.clone();
  m_ytransform = ty.clone();
}

RectangleTransformation::~RectangleTransformation() {
  if(m_xtransform != NULL) {
    delete m_xtransform;
  }
  if(m_ytransform != NULL) {
    delete m_ytransform;
  }
}

IntervalTransformation *RectangleTransformation::allocateTransformation(const DoubleInterval &from, const DoubleInterval &to, IntervalScale scale) {
  switch(scale) {
  case LINEAR             : return new LinearTransformation(from,to);
  case LOGARITHMIC        : return new LogarithmicTransformation(from,to);
  case NORMAL_DISTRIBUTION: return new NormalDistributionTransformation(from,to);
  default                 : throwException(_T("IntervalTransformation allocateTransformation:Invalid scale (=%d)"), scale);
                            return NULL;
  }
}

void RectangleTransformation::computeTransformation(const Rectangle2D &from, const Rectangle2D &to, IntervalScale xScale, IntervalScale yScale) {
  IntervalTransformation *newXtransform = NULL, *newYtransform = NULL;
  try {
    newXtransform = allocateTransformation(DoubleInterval(from.getBottomLeft().x,from.getBottomRight().x)
                                          ,DoubleInterval(to.getBottomLeft().x  ,to.getBottomRight().x)
                                          ,xScale);

    newYtransform = allocateTransformation(DoubleInterval(from.getTopLeft().y, from.getBottomLeft().y)
                                          ,DoubleInterval(to.getTopLeft().y  , to.getBottomLeft().y  )
                                          ,yScale);
    if(m_xtransform != NULL) {
      delete m_xtransform;
    }
    if(m_ytransform != NULL) {
      delete m_ytransform;
    }
    m_xtransform = newXtransform;
    m_ytransform = newYtransform;
  } catch(Exception e) {
    if(newXtransform != NULL) {
      delete newXtransform;
    }
    if(newYtransform != NULL) {
      delete newYtransform;
    }
    throw;
  }
}

void RectangleTransformation::setFromRectangle(const Rectangle2D &rect) {
  computeTransformation(rect,getToRectangle(),getXTransformation().getScale(),getYTransformation().getScale());
}

void RectangleTransformation::setToRectangle(const Rectangle2D &rect) {
  computeTransformation(getFromRectangle(),rect,getXTransformation().getScale(),getYTransformation().getScale());
}

Rectangle2D RectangleTransformation::getFromRectangle() const {
  Point2D p1 = Point2D(getXTransformation().getFromInterval().getFrom(), getYTransformation().getFromInterval().getFrom());
  Point2D p2 = Point2D(getXTransformation().getFromInterval().getTo()  , getYTransformation().getFromInterval().getTo());
  return Rectangle2D(p1.x, p1.y, p2.x-p1.x, p2.y-p1.y);
}

Rectangle2D RectangleTransformation::getToRectangle() const {
  Point2D p1 = Point2D(getXTransformation().getToInterval().getFrom()  , getYTransformation().getToInterval().getFrom());
  Point2D p2 = Point2D(getXTransformation().getToInterval().getTo()    , getYTransformation().getToInterval().getTo());
  return Rectangle2D(p1.x, p1.y, p2.x-p1.x, p2.y-p1.y);
}

Point2D RectangleTransformation::forwardTransform(const Point2D &p) const {
  return Point2D(getXTransformation().forwardTransform(p.x),getYTransformation().forwardTransform(p.y));
}

Point2D RectangleTransformation::forwardTransform(double x, double y) const {
  return Point2D(getXTransformation().forwardTransform(x),getYTransformation().forwardTransform(y));
}

Point2D RectangleTransformation::backwardTransform(const Point2D &p) const {
  return Point2D(getXTransformation().backwardTransform(p.x),getYTransformation().backwardTransform(p.y));
}

Point2D RectangleTransformation::backwardTransform(double x, double y) const {
  return Point2D(getXTransformation().backwardTransform(x),getYTransformation().backwardTransform(y));
}

Rectangle2D RectangleTransformation::forwardTransform(const Rectangle2D &rect) const {
  Point2D p1 = forwardTransform(rect.getTopLeft());
  Point2D p2 = forwardTransform(rect.getBottomRight());
  return Rectangle2D(p1.x,p1.y,p2.x-p1.x,p2.y-p1.y);
}

Rectangle2D RectangleTransformation::backwardTransform(const Rectangle2D &rect) const {
  Point2D p1 = backwardTransform(rect.getTopLeft());
  Point2D p2 = backwardTransform(rect.getBottomRight());
  return Rectangle2D(p1.x,p1.y,p2.x-p1.x,p2.y-p1.y);
}

void RectangleTransformation::setScale(IntervalScale newScale, int flags) {
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

Rectangle2D RectangleTransformation::zoom(const Point2D &p, double factor, int flags, bool pInToRectangle) {
  if(flags & X_AXIS) {
    m_xtransform->zoom(p.x, factor, pInToRectangle);
  }
  if(flags & Y_AXIS) {
    m_ytransform->zoom(p.y, factor, pInToRectangle);
  }
  return getFromRectangle();
}

bool RectangleTransformation::adjustAspectRatio() {
  if(!getXTransformation().isLinear() || !getYTransformation().isLinear()) {
    return false;
  }
  Rectangle2D fr = getFromRectangle();
  Rectangle2D tr = getToRectangle();
  double fromRatio = fabs(fr.getWidth() / fr.getHeight());
  double toRatio   = fabs(tr.getWidth() / tr.getHeight());
  bool changed = false;
  if(fromRatio / toRatio > 1) {
    double dh = getDouble(dsign(fr.getHeight())*(fabs(fr.getWidth()/toRatio) - fabs(fr.getHeight())));
    fr.m_y -= dh / 2;
    fr.m_h += dh;
    changed = dh != 0;
  } else if(fromRatio / toRatio < 1) {
    double dw = getDouble(dsign(fr.getWidth())*(fabs(toRatio*fr.getHeight()) - fabs(fr.getWidth())));
    fr.m_x -= dw / 2;
    fr.m_w += dw;
    changed = dw != 0;
  }
  if(changed) {
    setFromRectangle(fr);
  }
  return changed;
}

const RectangleTransformation RectangleTransformation::id(Rectangle2D(0,0,1,1),Rectangle2D(0,0,1,1));

// static
Rectangle2D RectangleTransformation::getDefaultFromRectangle(IntervalScale xScale, IntervalScale yScale) {
  DoubleInterval xInterval = IntervalTransformation::getDefaultInterval(xScale);
  DoubleInterval yInterval = IntervalTransformation::getDefaultInterval(yScale);
  return Rectangle2D(xInterval.getFrom(), yInterval.getFrom(), xInterval.getLength(), yInterval.getLength());
}

// static
Rectangle2D RectangleTransformation::getDefaultToRectangle() {
  return Rectangle2D(0, 100, 100, -100);
}
