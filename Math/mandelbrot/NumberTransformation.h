#pragma once

#include <NumberInterval.h>
#include "NumberPoint2D.h"
#include "NumberRectangle.h"

template <class T> class NumberIntervalTransformation {
private:
  NumberInterval<T> m_fromInterval;
  NumberInterval<T> m_toInterval;
  T m_a, m_b;

protected:
  virtual T translate(const T &x)        const = 0;
  virtual T inverseTranslate(const T &x) const = 0;

  void computeTransformation() {
    const NumberInterval<T> &toInterval   = getToInterval();
    const NumberInterval<T> &fromInterval = getFromInterval();
    m_a = toInterval.getLength() / (translate(fromInterval.getTo()) - translate(fromInterval.getFrom()));
    m_b = toInterval.getFrom() - m_a * translate(fromInterval.getFrom());
  }

  void checkFromInterval(const NumberInterval<T> &interval) const {
    if(interval.getLength() == 0) {
      throwException(_T("NumberIntervalTransformation::Invalid from-interval [%s;%s]")
        ,::toString(m_fromInterval.getFrom()).cstr()
        ,::toString(m_fromInterval.getTo()).cstr());
    }
  }

public:
  NumberIntervalTransformation(const NumberInterval<T> &fromInterval, const NumberInterval<T> &toInterval) {
    checkFromInterval(fromInterval);
    m_fromInterval = fromInterval;
    m_toInterval   = toInterval;
  }

  static NumberInterval<T> getDefaultInterval() {
    return NumberInterval<T>(-10,10);
  }

  const NumberInterval<T> &getFromInterval() const {
    return m_fromInterval;
  }
  const NumberInterval<T> &getToInterval() const {
    return m_toInterval;
  }
  
  virtual const NumberInterval<T> &setFromInterval(  const NumberInterval<T> &interval) {
    checkFromInterval(interval);
    m_fromInterval = interval;
    computeTransformation();
    return m_fromInterval;
  }

  virtual const NumberInterval<T> &setToInterval(    const NumberInterval<T> &interval) {
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
    return NumberInterval<T>(forwardTransform(interval.getFrom()), forwardTransform(interval.getTo()));
  }

  NumberInterval<T> backwardTransform(const NumberInterval<T> &interval) const {
    return NumberInterval<T>(backwardTransform(interval.getFrom()), backwardTransform(interval.getTo()));
  }

  const NumberInterval<T> &zoom(const T &x, const T &factor, bool xInToInterval=true) { // Returns new fromInterval.
    if(xInToInterval) {
      if(!getToInterval().contains(x)) {
        return m_fromInterval;
      }
    } else if(!getFromInterval().contains(x)) {
      return m_fromInterval;
    }
    T x1     = xInToInterval ? translate(backwardTransform(x)) : x;
    T tFrom  = translate(getFromInterval().getFrom());
    T tTo    = translate(getFromInterval().getTo());
    tFrom += (x1 - tFrom) * factor;
    tTo   += (x1 - tTo  ) * factor;
    return setFromInterval(NumberInterval<T>(inverseTranslate(tFrom), inverseTranslate(tTo)));
  }
  virtual NumberIntervalTransformation<T> *clone() const = 0;
  virtual ~NumberIntervalTransformation() {
  }
};

template <class T> class LinearNumberTransformation : public NumberIntervalTransformation<T> {
protected:
  T translate(const T &x) const {
    return x;
  }
  T inverseTranslate(const T &x) const {
    return x;
  }

public:
  LinearNumberTransformation(const NumberInterval<T> &fromInterval, const NumberInterval<T> &toInterval)
    :NumberIntervalTransformation<T>(fromInterval, toInterval)
  {
    computeTransformation();
  }
  bool isLinear() const {
    return true;
  }
  NumberIntervalTransformation<T> *clone() const {
    return new LinearNumberTransformation<T>(*this);
  }
};

template <class T> class LogarithmicNumberTransformation : public NumberIntervalTransformation<T> {
protected:
  T translate(const T &x) const {
    if(x <= 0) {
      throwException(_T("Negative number not allowed in logarithmic scale"));
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
  LogarithmicNumberTransformation(const NumberInterval<T> &fromInterval, const NumberInterval<T> &toInterval)
    :NumberIntervalTransformation<T>(fromInterval, toInterval)
  {
    computeTransformation();
  }
  NumberIntervalTransformation<T> *clone() const {
    return new LogarithmicNumberTransformation<T>(*this);
  }
};

template <class T> class ExponentialNumberTransformation : public NumberIntervalTransformation<T> {
protected:
  T translate(const T &x) const {
    return exp(x);
  }

  T inverseTranslate(const T &x) const {
    if(x <= 0) {
      throwException(_T("Negative number not allowed in logarithmic scale"));
    }
    return log(x);
  }
public:
  bool isLinear() const {
    return false;
  }
  ExponentialNumberTransformation(const NumberInterval<T> &fromInterval, const NumberInterval<T> &toInterval)
    :NumberIntervalTransformation<T>(fromInterval, toInterval)
  {
    computeTransformation();
  }
  NumberIntervalTransformation<T> *clone() const {
    return new ExponentialNumberTransformation<T>(*this);
  }
};

#define X_AXIS 1
#define Y_AXIS 2

template <class T> class NumberRectangleTransformation {
private:
  NumberIntervalTransformation<T> *m_xtransform;
  NumberIntervalTransformation<T> *m_ytransform;

  void computeTransformation(const NumberRectangle<T> &from, const NumberRectangle<T> &to) {
    NumberIntervalTransformation<T> *newXtransform = NULL, *newYtransform = NULL;
    try {
      newXtransform = allocateTransformation(NumberInterval<T>(from.getTopLeft().x, from.getBottomRight().x)
                                            ,NumberInterval<T>(to.getTopLeft().x  , to.getBottomRight().x  )
                                            );

      newYtransform = allocateTransformation(NumberInterval<T>(from.getTopLeft().y, from.getBottomRight().y)
                                            ,NumberInterval<T>(to.getTopLeft().y  , to.getBottomRight().y  )
                                            );
      cleanup();
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

  void cleanup() {
    if(m_xtransform != NULL) {
      delete m_xtransform;
      m_xtransform = NULL;
    }
    if(m_ytransform != NULL) {
      delete m_ytransform;
      m_ytransform = NULL;
    }
  }

  NumberIntervalTransformation<T> *allocateTransformation(const NumberInterval<T> &from, const NumberInterval<T> &to) const {
    return new LinearNumberTransformation<T>(from, to);
  }

  NumberRectangleTransformation(const NumberIntervalTransformation<T> &tx, const NumberIntervalTransformation<T> &ty) {
    m_xtransform = tx.clone();
    m_ytransform = ty.clone();
  }

  NumberRectangle<T> getDefaultFromRectangle() {
    const NumberInterval<T> xInterval = NumberIntervalTransformation<T>::getDefaultInterval();
    const NumberInterval<T> yInterval = NumberIntervalTransformation<T>::getDefaultInterval();
    return NumberRectangle<T>(xInterval.getFrom(), yInterval.getFrom(), xInterval.getLength(), yInterval.getLength());
  }
  NumberRectangle<T> getDefaultToRectangle() const {
    return NumberRectangle<T>(0, 100, 100, -100);
  }

public:
  NumberRectangleTransformation() {
    m_xtransform = m_ytransform = NULL;
    computeTransformation(getDefaultFromRectangle(), getDefaultToRectangle());
  }

  NumberRectangleTransformation(const NumberRectangleTransformation<T> &src) {
    m_xtransform = src.getXTransformation().clone();
    m_ytransform = src.getYTransformation().clone();
  }

  NumberRectangleTransformation<T> &operator=(const NumberRectangleTransformation<T> &src) {
    cleanup();
    m_xtransform = src.getXTransformation().clone();
    m_ytransform = src.getYTransformation().clone();
    return *this;
  }

  NumberRectangleTransformation(const NumberRectangle<T> &from, const NumberRectangle<T> &to) {
    m_xtransform = m_ytransform = NULL;
    computeTransformation(from, to, xScale, yScale);
  }

  ~NumberRectangleTransformation() {
    cleanup();
  }

  inline const NumberIntervalTransformation<T> &getXTransformation() const {
    return *m_xtransform;
  }
  inline const NumberIntervalTransformation<T> &getYTransformation() const {
    return *m_ytransform;
  }

  inline void setFromRectangle(const NumberRectangle<T> &rect) {
    computeTransformation(rect, getToRectangle());
  }

  inline void setToRectangle(const NumberRectangle<T> &rect) {
    computeTransformation(getFromRectangle(), rect);
  }

  NumberRectangle<T> getFromRectangle() const {
    const NumberPoint2D<T> p1 = NumberPoint2D<T>(getXTransformation().getFromInterval().getFrom(), getYTransformation().getFromInterval().getFrom());
    const NumberPoint2D<T> p2 = NumberPoint2D<T>(getXTransformation().getFromInterval().getTo()  , getYTransformation().getFromInterval().getTo());
    return NumberRectangle<T>(p1.x, p1.y, p2.x-p1.x, p2.y-p1.y);
  }

  NumberRectangle<T> getToRectangle() const {
    const NumberPoint2D<T> p1 = NumberPoint2D<T>(getXTransformation().getToInterval().getFrom()  , getYTransformation().getToInterval().getFrom());
    const NumberPoint2D<T> p2 = NumberPoint2D<T>(getXTransformation().getToInterval().getTo()    , getYTransformation().getToInterval().getTo());
    return NumberRectangle<T>(p1.x, p1.y, p2.x-p1.x, p2.y-p1.y);
  }

  inline NumberPoint2D<T> forwardTransform(const NumberPoint2D<T> &p) const {
    return NumberPoint2D<T>(getXTransformation().forwardTransform(p.x), getYTransformation().forwardTransform(p.y));
  }

  inline NumberPoint2D<T> backwardTransform(const NumberPoint2D<T> &p) const {
    return NumberPoint2D<T>(getXTransformation().backwardTransform(p.x), getYTransformation().backwardTransform(p.y));
  }

  inline NumberPoint2D<T> forwardTransform(const T &x, const T &y) const {
    return NumberPoint2D<T>(getXTransformation().forwardTransform(x), getYTransformation().forwardTransform(y));
  }

  inline NumberPoint2D<T> backwardTransform(const T &x, const T &y) const {
    return NumberPoint2D<T>(getXTransformation().backwardTransform(x), getYTransformation().backwardTransform(y));
  }

  NumberRectangle<T> forwardTransform(const NumberRectangle<T> &rect) const {
    const NumberPoint2D<T> p1 = forwardTransform(rect.getTopLeft());
    const NumberPoint2D<T> p2 = forwardTransform(rect.getBottomRight());
    return NumberRectangle<T>(p1.x,p1.y,p2.x-p1.x,p2.y-p1.y);
  }

  NumberRectangle<T> backwardTransform(const NumberRectangle<T> &rect) const {
    const NumberPoint2D<T> p1 = backwardTransform(rect.getTopLeft());
    const NumberPoint2D<T> p2 = backwardTransform(rect.getBottomRight());
    return NumberRectangle<T>(p1.x,p1.y,p2.x-p1.x,p2.y-p1.y);
  }

  NumberRectangle<T> zoom(const NumberPoint2D<T> &p, T factor, int flags = X_AXIS | Y_AXIS, bool pInToRectangle=true) { // Returns new fromRectangle.
    if(flags & X_AXIS) {
      m_xtransform->zoom(p.x, factor, pInToRectangle);
    }
    if(flags & Y_AXIS) {
      m_ytransform->zoom(p.y, factor, pInToRectangle);
    }
    return getFromRectangle();
  }

  bool adjustAspectRatio() { // returns true if transformation is changed
    NumberRectangle<T> fr = getFromRectangle();
    NumberRectangle<T> tr = getToRectangle();
    const T fromRatio = fabs(fr.getWidth() / fr.getHeight());
    const T toRatio   = fabs(tr.getWidth() / tr.getHeight());
    bool changed = false;
    if(fromRatio / toRatio > 1) {
      const T dh = sign(fr.getHeight())*(fabs(fr.getWidth()/toRatio) - fabs(fr.getHeight()));
      fr.y -= dh / 2;
      fr.h += dh;
      changed = dh != 0;
    } else if(fromRatio / toRatio < 1) {
      const T dw = sign(fr.getWidth())*(fabs(toRatio*fr.getHeight()) - fabs(fr.getWidth()));
      fr.x -= dw / 2;
      fr.w += dw;
      changed = dw != 0;
    }
    if(changed) {
      setFromRectangle(fr);
    }
    return changed;
  }
};
