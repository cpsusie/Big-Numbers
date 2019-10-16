#pragma once

#include <NumberInterval.h>
#include "Point2D.h"
#include "Rectangle2D.h"
#include "PragmaLib.h"

typedef enum {
  LINEAR
 ,LOGARITHMIC
 ,NORMAL_DISTRIBUTION
} IntervalScale;

template<class T> class IntervalTransformationTemplate {
private:
  NumberInterval<T> m_fromInterval, m_toInterval;
  T m_a, m_b;

protected:
  virtual T translate(       const T &x) const = 0;
  virtual T inverseTranslate(const T &x) const = 0;
  void computeTransformation() {
    const NumberInterval<T> &toInterval   = getToInterval();
    const NumberInterval<T> &fromInterval = getFromInterval();
    m_a = toInterval.getLength() / (translate(fromInterval.getTo()) - translate(fromInterval.getFrom()));
    m_b = toInterval.getFrom() - m_a * translate(fromInterval.getFrom());
  }
  void checkFromInterval(const NumberInterval<T> &interval) {
    if(interval.getLength() == 0) {
      throwException(_T("IntervalTransformation::Invalid from-interval:%s"),interval.toString().cstr());
    }
  }
public:
  IntervalTransformationTemplate(const NumberInterval<T> &fromInterval, const NumberInterval<T> &toInterval) {
    checkFromInterval(fromInterval);
    m_fromInterval = fromInterval;
    m_toInterval   = toInterval;
  }
  static NumberInterval<T> getDefaultInterval(IntervalScale scale) {
    switch(scale) {
    case LINEAR             : return NumberInterval<T>(-10,10);
    case LOGARITHMIC        : return NumberInterval<T>(1,10);
    case NORMAL_DISTRIBUTION: return NumberInterval<T>(1e-6,1-1e-6);
    default                 : throwInvalidArgumentException(__TFUNCTION__
                                                           ,_T("scale (=%d)"), scale);
                              return NumberInterval<T>(0,10);
    }
  }

  const NumberInterval<T> &getFromInterval() const {
    return m_fromInterval;
  }
  const NumberInterval<T> &getToInterval() const {
    return m_toInterval;
  }
  virtual const NumberInterval<T> &setFromInterval(const NumberInterval<T> &interval) {
    checkFromInterval(interval);
    m_fromInterval = interval;
    computeTransformation();
    return m_fromInterval;
  }
  virtual const NumberInterval<T> &setToInterval(const NumberInterval<T> &interval) {
    m_toInterval = interval;
    computeTransformation();
    return m_toInterval;
  }

  T forwardTransform(const T &x) const {
    return m_a * translate(x) + m_b;
  }
  T backwardTransform(const T &x) const {
    return (m_a == 0) ? getFromInterval().getFrom() : inverseTranslate((x - m_b) / m_a);
  }
  NumberInterval<T> forwardTransform(const NumberInterval<T> &interval) const {
    return NumberInterval<T>(forwardTransform(interval.getFrom()),forwardTransform(interval.getTo()));
  }
  NumberInterval<T> backwardTransform(const NumberInterval<T> &interval) const {
    return NumberInterval<T>(backwardTransform(interval.getFrom()),backwardTransform(interval.getTo()));
  }

  virtual bool isLinear() const = 0;
  // Returns new fromInterval.
  const NumberInterval<T> &zoom(const T &x, const T &factor, bool xInToInterval=true) {
    if(xInToInterval) {
      if(!getToInterval().contains(x)) {
        return m_fromInterval;
      }
    } else {
      if(!getFromInterval().contains(x)) {
        return m_fromInterval;
      }
    }
    T x1     = xInToInterval ? translate(backwardTransform(x)) : x;
    T tFrom  = translate(getFromInterval().getFrom());
    T tTo    = translate(getFromInterval().getTo());
    tFrom += (x1 - tFrom) * factor;
    tTo   += (x1 - tTo  ) * factor;
    return setFromInterval(NumberInterval<T>(inverseTranslate(tFrom), inverseTranslate(tTo)));
  }
  bool operator==(const IntervalTransformationTemplate &rhs) const {
    return (getScale()        == rhs.getScale()       )
        && (getFromInterval() == rhs.getFromInterval())
        && (getToInterval()   == rhs.getToInterval()  );
  }
  bool operator!=(const IntervalTransformationTemplate &rhs) const {
    return !(*this == rhs);
  }
  virtual IntervalScale getScale() const = 0;
  virtual IntervalTransformationTemplate *clone() const = 0;
  virtual ~IntervalTransformationTemplate() {
  }
};

typedef IntervalTransformationTemplate<float   > FloatIntervalTransformation;
typedef IntervalTransformationTemplate<double  > IntervalTransformation;
typedef IntervalTransformationTemplate<Double80> D80IntervalTransformation;
typedef IntervalTransformationTemplate<Real    > RealIntervalTransformation;

template<class T> class LinearTransformationTemplate : public IntervalTransformationTemplate<T> {
protected:
  T translate(const T &x) const {
    return x;
  }
  T inverseTranslate(const T &x) const {
    return x;
  }
public:
  bool isLinear() const {
    return true;
  }
  LinearTransformationTemplate(const NumberInterval<T> &fromInterval, const NumberInterval<T> &toInterval)
    : IntervalTransformationTemplate<T>(fromInterval, toInterval) {
    computeTransformation();
  }
  IntervalScale getScale() const {
    return LINEAR;
  }
  IntervalTransformationTemplate<T> *clone() const {
    return new LinearTransformationTemplate(*this);
  }
};

typedef LinearTransformationTemplate<float   > FloatLinearTransformation;
typedef LinearTransformationTemplate<double  > LinearTransformation;
typedef LinearTransformationTemplate<Double80> D80LinearTransformation;
typedef LinearTransformationTemplate<Real    > RealLinearTransformation;

template<class T> class LogarithmicTransformationTemplate : public IntervalTransformationTemplate<T> {
protected:
  T translate(const T &x) const {
    if(x <= 0) {
      throwInvalidArgumentException(__TFUNCTION__, _T("x=%s"), ::toString(x).cstr());
    }
    return log(x);
  }
  T inverseTranslate(const T &x) const {
    return exp(x);
  }
public:
  bool isLinear() const {
    return false;
  }
  LogarithmicTransformationTemplate(const NumberInterval<T> &fromInterval, const NumberInterval<T> &toInterval)
    : IntervalTransformationTemplate<T>(fromInterval, toInterval) {
    computeTransformation();
  }
  IntervalScale getScale() const {
    return LOGARITHMIC;
  }
  IntervalTransformationTemplate<T> *clone() const {
    return new LogarithmicTransformationTemplate(*this);
  }
};

typedef LogarithmicTransformationTemplate<float   > FloatLogarithmicTransformation;
typedef LogarithmicTransformationTemplate<double  > LogarithmicTransformation;
typedef LogarithmicTransformationTemplate<Double80> D80LogarithmicTransformation;
typedef LogarithmicTransformationTemplate<Real    > RealLogarithmicTransformation;

template<class T> class NormalDistributionTransformationTemplate : public IntervalTransformationTemplate<T> {
protected:
  T translate(const T &x) const {
    if(x <= 0 || x >= 1) {
      throwInvalidArgumentException(__TFUNCTION__, _T("x=%s outside valid interval ]0;1["),::toString(x).cstr());
    }
    return probitFunction(x);
  }

  T inverseTranslate(const T &x) const {
    return norm(x);
  }

  const NumberInterval<T> &setFromInterval(const NumberInterval<T> &i) {
    NumberInterval<T> n = i.interSection(NumberInterval<T>(1e-12,1 - 1e-12));
    if(n.getLength() == 0) {
      n = NumberInterval<T>(1e-12,1 - 1e-12);
    }
    return __super::setFromInterval(n);
  }
public:
  NormalDistributionTransformationTemplate(const NumberInterval<T> &fromInterval, const NumberInterval<T> &toInterval)
    : IntervalTransformationTemplate<T>(fromInterval, toInterval)
  {
    computeTransformation();
  }
  bool isLinear() const {
    return false;
  }
  IntervalScale getScale() const {
    return NORMAL_DISTRIBUTION;
  }
  IntervalTransformationTemplate<T> *clone() const {
    return new NormalDistributionTransformationTemplate(*this);
  }
};

typedef NormalDistributionTransformationTemplate<float   > FloatNormalDistributionTransformation;
typedef NormalDistributionTransformationTemplate<double  > NormalDistributionTransformation;
typedef NormalDistributionTransformationTemplate<Double80> D80NormalDistributionTransformation;
typedef NormalDistributionTransformationTemplate<Real    > RealNormalDistributionTransformation;

#define X_AXIS 1
#define Y_AXIS 2

template<class T> class RectangleTransformationTemplate {
private:
  IntervalTransformationTemplate<T> *m_xtransform, *m_ytransform;
  IntervalTransformationTemplate<T> *allocateTransformation(const NumberInterval<T> &from, const NumberInterval<T> &to, IntervalScale scale) {
    switch(scale) {
    case LINEAR             : return new LinearTransformationTemplate<T>(            from, to);
    case LOGARITHMIC        : return new LogarithmicTransformationTemplate<T>(       from, to);
    case NORMAL_DISTRIBUTION: return new NormalDistributionTransformationTemplate<T>(from, to);
    default                 : throwInvalidArgumentException(__TFUNCTION__, _T("scale=%d"), scale);
                              return NULL;
    }
  }
  void cleanup() {
    SAFEDELETE(m_xtransform);
    SAFEDELETE(m_ytransform);
  }
  void computeTransformation(const Rectangle2DTemplate<T> &from, const Rectangle2DTemplate<T> &to, IntervalScale xScale, IntervalScale yScale) {
    IntervalTransformationTemplate<T> *newXtransform = NULL, *newYtransform = NULL;
    try {
      newXtransform = allocateTransformation(NumberInterval<T>(from.getBottomLeft().x,from.getBottomRight().x)
                                            ,NumberInterval<T>(to.getBottomLeft().x  ,to.getBottomRight().x)
                                            ,xScale);

      newYtransform = allocateTransformation(NumberInterval<T>(from.getTopLeft().y, from.getBottomLeft().y)
                                            ,NumberInterval<T>(to.getTopLeft().y  , to.getBottomLeft().y  )
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

  RectangleTransformationTemplate(const IntervalTransformationTemplate<T> &tx, const IntervalTransformationTemplate<T> &ty) {
    m_xtransform = tx.clone();
    m_ytransform = ty.clone();
  }

  static Rectangle2DTemplate<T> getDefaultFromRectangle(IntervalScale xScale, IntervalScale yScale) {
    NumberInterval<T> xInterval = IntervalTransformationTemplate<T>::getDefaultInterval(xScale);
    NumberInterval<T> yInterval = IntervalTransformationTemplate<T>::getDefaultInterval(yScale);
    return Rectangle2DTemplate<T>(xInterval.getFrom(), yInterval.getFrom(), xInterval.getLength(), yInterval.getLength());
  }
  static Rectangle2DTemplate<T> getDefaultToRectangle() {
    return Rectangle2DTemplate<T>(0, 100, 100, -100);
  }

public:
  RectangleTransformationTemplate(IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR) {
    m_xtransform = m_ytransform = NULL;
    computeTransformation(getDefaultFromRectangle(xScale,yScale), getDefaultToRectangle(), xScale, yScale);
  }
  RectangleTransformationTemplate(const RectangleTransformationTemplate &src) {
    m_xtransform = src.getXTransformation().clone();
    m_ytransform = src.getYTransformation().clone();
  }
  RectangleTransformationTemplate &operator=(const RectangleTransformationTemplate &src) {
    cleanup();
    m_xtransform = src.getXTransformation().clone();
    m_ytransform = src.getYTransformation().clone();
    return *this;
  }
  RectangleTransformationTemplate(const Rectangle2DTemplate<T> &from, const Rectangle2DTemplate<T> &to, IntervalScale xScale = LINEAR, IntervalScale yScale = LINEAR) {
    m_xtransform = m_ytransform = NULL;
    computeTransformation(from, to, xScale, yScale);
  }
  virtual ~RectangleTransformationTemplate() {
    cleanup();
  }
  const IntervalTransformationTemplate<T> &getXTransformation() const {
    return *m_xtransform;
  }
  const IntervalTransformationTemplate<T> &getYTransformation() const {
    return *m_ytransform;
  }

  void   setFromRectangle(const Rectangle2DTemplate<T> &rect) {
    computeTransformation(rect,getToRectangle(),getXTransformation().getScale(),getYTransformation().getScale());
  }
  void   setToRectangle(const Rectangle2DTemplate<T> &rect) {
    computeTransformation(getFromRectangle(),rect,getXTransformation().getScale(),getYTransformation().getScale());
  }
  Rectangle2DTemplate<T>  getFromRectangle() const {
    Point2DTemplate<T> p1 = Point2DTemplate<T>(getXTransformation().getFromInterval().getFrom(), getYTransformation().getFromInterval().getFrom());
    Point2DTemplate<T> p2 = Point2DTemplate<T>(getXTransformation().getFromInterval().getTo()  , getYTransformation().getFromInterval().getTo());
    return Rectangle2DTemplate<T>(p1.x, p1.y, p2.x-p1.x, p2.y-p1.y);
  }
  Rectangle2DTemplate<T>  getToRectangle()   const {
    Point2DTemplate<T> p1 = Point2DTemplate<T>(getXTransformation().getToInterval().getFrom()  , getYTransformation().getToInterval().getFrom());
    Point2DTemplate<T> p2 = Point2DTemplate<T>(getXTransformation().getToInterval().getTo()    , getYTransformation().getToInterval().getTo());
    return Rectangle2DTemplate<T>(p1.x, p1.y, p2.x-p1.x, p2.y-p1.y);
  }
  Point2DTemplate<T>      forwardTransform(const Point2DTemplate<T> &p)   const {
    return Point2DTemplate<T>(getXTransformation().forwardTransform(p.x),getYTransformation().forwardTransform(p.y));
  }
  Point2DTemplate<T>      backwardTransform(const Point2DTemplate<T> &p)   const {
    return Point2DTemplate<T>(getXTransformation().backwardTransform(p.x),getYTransformation().backwardTransform(p.y));
  }
  Point2DTemplate<T>      forwardTransform(const T &x, const T &y) const {
    return Point2DTemplate<T>(getXTransformation().forwardTransform(x),getYTransformation().forwardTransform(y));
  }
  Point2DTemplate<T>      backwardTransform(const T &x, const T &y) const {
    return Point2DTemplate<T>(getXTransformation().backwardTransform(x),getYTransformation().backwardTransform(y));
  }
  Rectangle2DTemplate<T>  forwardTransform(const Rectangle2DTemplate<T> &rect)  const {
    Point2DTemplate<T> p1 = forwardTransform(rect.getTopLeft());
    Point2DTemplate<T> p2 = forwardTransform(rect.getBottomRight());
    return Rectangle2DTemplate<T>(p1.x,p1.y,p2.x-p1.x,p2.y-p1.y);
  }
  Rectangle2DTemplate<T>  backwardTransform( const Rectangle2DTemplate<T> &rect)  const {
    Point2DTemplate<T> p1 = backwardTransform(rect.getTopLeft());
    Point2DTemplate<T> p2 = backwardTransform(rect.getBottomRight());
    return Rectangle2DTemplate<T>(p1.x,p1.y,p2.x-p1.x,p2.y-p1.y);
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
  Rectangle2DTemplate<T> zoom(const Point2DTemplate<T> &p, const T &factor, int flags = X_AXIS | Y_AXIS, bool pInToRectangle=true) {
    if(flags & X_AXIS) {
      m_xtransform->zoom(p.x, factor, pInToRectangle);
    }
    if(flags & Y_AXIS) {
      m_ytransform->zoom(p.y, factor, pInToRectangle);
    }
    return getFromRectangle();
  }
  // returns true if transformation is changed
  bool adjustAspectRatio() {
    if(!getXTransformation().isLinear() || !getYTransformation().isLinear()) {
      return false;
    }
    Rectangle2DTemplate<T> fr        = getFromRectangle();
    Rectangle2DTemplate<T> tr        = getToRectangle();
    const T                fromRatio = fabs(fr.getWidth() / fr.getHeight());
    const T                toRatio   = fabs(tr.getWidth() / tr.getHeight());
    bool                   changed   = false;
    if(fromRatio > toRatio) {
      const T dh = dsign(fr.getHeight())*(fabs(fr.getWidth()/toRatio) - fabs(fr.getHeight()));
      fr.m_y -= dh / 2;
      fr.m_h += dh;
      changed = dh != 0;
    } else if(fromRatio < toRatio) {
      const T dw = dsign(fr.getWidth())*(fabs(toRatio*fr.getHeight()) - fabs(fr.getWidth()));
      fr.m_x -= dw / 2;
      fr.m_w += dw;
      changed = dw != 0;
    }
    if(changed) {
      setFromRectangle(fr);
    }
    return changed;
  }

  bool operator==(const RectangleTransformationTemplate<T> &rhs) const {
    return (getXTransformation() == rhs.getXTransformation())
        && (getYTransformation() == rhs.getYTransformation());
  }
  bool operator!=(const RectangleTransformationTemplate<T> &rhs) const {
    return !(*this == rhs);
  }
  static RectangleTransformationTemplate<T> getId() {
    return RectangleTransformationTemplate(Rectangle2DTemplate<T>(0,0,1,1)
                                          ,Rectangle2DTemplate<T>(0,0,1,1));
  }
};

typedef RectangleTransformationTemplate<float   > FloatRectangleTransformation;
typedef RectangleTransformationTemplate<double  > RectangleTransformation;
typedef RectangleTransformationTemplate<Double80> D80RectangleTransformation;
typedef RectangleTransformationTemplate<Real    > RealRectangleTransformation;
