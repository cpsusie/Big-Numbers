#pragma once

#include <NumberInterval.h>
#include "Real.h"

typedef enum {
  LINEAR
 ,LOGARITHMIC
 ,NORMAL_DISTRIBUTION
} IntervalScale;

template<typename T> class IntervalTransformationTemplate {
private:
  const IntervalScale m_scaleType;
  NumberInterval<T>   m_fromInterval, m_toInterval;
  T                   m_a, m_b;

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
      throwInvalidArgumentException(__TFUNCTION__, _T("From interval=%s"),interval.toString().cstr());
    }
  }
public:
  IntervalTransformationTemplate(const NumberInterval<T> &fromInterval, const NumberInterval<T> &toInterval, IntervalScale scaleType)
    : m_scaleType(scaleType)
  {
    checkFromInterval(fromInterval);
    m_fromInterval = fromInterval;
    m_toInterval   = toInterval;
  }
  static NumberInterval<T> getDefaultFromInterval(IntervalScale scale) {
    switch(scale) {
    case LINEAR             : return NumberInterval<T>(-10,10);
    case LOGARITHMIC        : return NumberInterval<T>(  1,10);
    case NORMAL_DISTRIBUTION: return NumberInterval<T>(1e-6,1-1e-6);
    default                 : throwInvalidArgumentException(__TFUNCTION__,_T("scale (=%d)"), scale);
                              return NumberInterval<T>(0,10);
    }
  }

  IntervalScale getScaleType() const {
    return m_scaleType;
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

  template<typename S> T forwardTransform(const S &x) const {
    const T xT = (T)x;
    return m_a * translate(xT) + m_b;
  }
  template<typename S> T backwardTransform(const S &x) const {
    const T xT = (T)x;
    return (m_a == 0) ? getFromInterval().getFrom() : inverseTranslate((xT - m_b) / m_a);
  }
  template<typename S> NumberInterval<T> forwardTransform(const NumberInterval<S> &interval) const {
    return NumberInterval<T>(forwardTransform(interval.getFrom()),forwardTransform(interval.getTo()));
  }
  template<typename S> NumberInterval<T> backwardTransform(const NumberInterval<S> &interval) const {
    return NumberInterval<T>(backwardTransform(interval.getFrom()),backwardTransform(interval.getTo()));
  }

  // Returns new fromInterval.
  template<typename T1, typename T2> const NumberInterval<T> &zoom(const T1 &x, const T2 &factor, bool xInToInterval=true) {
    const T xT = (T)x;
    if(xInToInterval) {
      if(!getToInterval().contains(xT)) {
        return m_fromInterval;
      }
    } else if(!getFromInterval().contains(xT)) {
      return m_fromInterval;
    }
    T x1      = xInToInterval ? translate(backwardTransform(xT)) : xT;
    T tFrom   = translate(getFromInterval().getFrom());
    T tTo     = translate(getFromInterval().getTo());
    T factorT = (T)factor;
    tFrom += (x1 - tFrom) * factorT;
    tTo   += (x1 - tTo  ) * factorT;
    return setFromInterval(NumberInterval<T>(inverseTranslate(tFrom), inverseTranslate(tTo)));
  }
  bool operator==(const IntervalTransformationTemplate &rhs) const {
    return (getScaleType()    == rhs.getScaleType()   )
        && (getFromInterval() == rhs.getFromInterval())
        && (getToInterval()   == rhs.getToInterval()  );
  }
  bool operator!=(const IntervalTransformationTemplate &rhs) const {
    return !(*this == rhs);
  }
  virtual IntervalTransformationTemplate *clone()    const = 0;
  virtual bool                            isLinear() const = 0;
  virtual ~IntervalTransformationTemplate() {
  }
};

typedef IntervalTransformationTemplate<float   > FloatIntervalTransformation;
typedef IntervalTransformationTemplate<double  > IntervalTransformation;
typedef IntervalTransformationTemplate<Real    > RealIntervalTransformation;

template<typename T> class LinearTransformationTemplate : public IntervalTransformationTemplate<T> {
protected:
  T translate(const T &x) const override {
    return x;
  }
  T inverseTranslate(const T &x) const override {
    return x;
  }
public:
  LinearTransformationTemplate(const NumberInterval<T> &fromInterval, const NumberInterval<T> &toInterval)
    : IntervalTransformationTemplate<T>(fromInterval, toInterval, LINEAR) {
    computeTransformation();
  }
  IntervalTransformationTemplate<T> *clone() const override {
    IntervalTransformationTemplate<T> *t = new LinearTransformationTemplate(*this); TRACE_NEW(t);
    return t;
  }
  bool isLinear() const override {
    return true;
  }
};

typedef LinearTransformationTemplate<float   > FloatLinearTransformation;
typedef LinearTransformationTemplate<double  > LinearTransformation;
typedef LinearTransformationTemplate<Real    > RealLinearTransformation;

template<typename T> class LogarithmicTransformationTemplate : public IntervalTransformationTemplate<T> {
protected:
  T translate(const T &x) const override {
    if(x <= 0) {
      throwInvalidArgumentException(__TFUNCTION__, _T("x=%s"), ::toString(x).cstr());
    }
    return log(x);
  }
  T inverseTranslate(const T &x) const override {
    return exp(x);
  }
public:
  LogarithmicTransformationTemplate(const NumberInterval<T> &fromInterval, const NumberInterval<T> &toInterval)
    : IntervalTransformationTemplate<T>(fromInterval, toInterval, LOGARITHMIC) {
    computeTransformation();
  }
  IntervalTransformationTemplate<T> *clone() const override {
    IntervalTransformationTemplate<T> *t = new LogarithmicTransformationTemplate(*this); TRACE_NEW(t);
    return t;
  }
  bool isLinear() const override {
    return false;
  }
};
typedef LogarithmicTransformationTemplate<float   > FloatLogarithmicTransformation;
typedef LogarithmicTransformationTemplate<double  > LogarithmicTransformation;
typedef LogarithmicTransformationTemplate<Real    > RealLogarithmicTransformation;

template<typename T> class NormalDistributionTransformationTemplate : public IntervalTransformationTemplate<T> {
protected:
  T translate(const T &x) const override {
    if(x <= 0 || x >= 1) {
      throwInvalidArgumentException(__TFUNCTION__, _T("x=%s outside valid interval ]0;1["),::toString(x).cstr());
    }
    return probitFunction(x);
  }

  T inverseTranslate(const T &x) const override {
    return gaussDistribution(x);
  }

  const NumberInterval<T> &setFromInterval(const NumberInterval<T> &i) override {
    NumberInterval<T> n = i.interSection(NumberInterval<T>(1e-12,1 - 1e-12));
    if(n.getLength() == 0) {
      n = NumberInterval<T>(1e-12,1 - 1e-12);
    }
    return __super::setFromInterval(n);
  }
public:
  NormalDistributionTransformationTemplate(const NumberInterval<T> &fromInterval, const NumberInterval<T> &toInterval)
    : IntervalTransformationTemplate<T>(fromInterval, toInterval, NORMAL_DISTRIBUTION)
  {
    computeTransformation();
  }
  IntervalTransformationTemplate<T> *clone() const override {
    IntervalTransformationTemplate<T> *t = new NormalDistributionTransformationTemplate(*this); TRACE_NEW(t);
    return t;
  }
  bool isLinear() const override {
    return false;
  }
};

typedef NormalDistributionTransformationTemplate<float   > FloatNormalDistributionTransformation;
typedef NormalDistributionTransformationTemplate<double  > NormalDistributionTransformation;
typedef NormalDistributionTransformationTemplate<Real    > RealNormalDistributionTransformation;

#define X_AXIS 1
#define Y_AXIS 2
#define Z_AXIS 4 /* only used for CubeTransformationTemplate */

template<typename T> IntervalTransformationTemplate<T> *allocateIntervalTransformation(const NumberInterval<T> &from, const NumberInterval<T> &to, IntervalScale scale) {
  IntervalTransformationTemplate<T> *result;
  switch(scale) {
  case LINEAR             : result = new LinearTransformationTemplate<T>(            from, to); TRACE_NEW(result); break;
  case LOGARITHMIC        : result = new LogarithmicTransformationTemplate<T>(       from, to); TRACE_NEW(result); break;
  case NORMAL_DISTRIBUTION: result = new NormalDistributionTransformationTemplate<T>(from, to); TRACE_NEW(result); break;
  default                 : throwInvalidArgumentException(__TFUNCTION__, _T("scale=%d"), scale);
                            return nullptr;
  }
  return result;
}
