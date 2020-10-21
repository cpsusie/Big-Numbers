#pragma once

#include "Double80.h"
#include "Real.h"

typedef enum {
  LINEAR
 ,LOGARITHMIC
 ,NORMAL_DISTRIBUTION
} IntervalScale;

template<typename T> class IntervalTransformationTemplate {
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
      throwInvalidArgumentException(__TFUNCTION__, _T("From interval=%s"),interval.toString().cstr());
    }
  }
public:
  IntervalTransformationTemplate(const NumberInterval<T> &fromInterval, const NumberInterval<T> &toInterval) {
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

template<typename T> class LinearTransformationTemplate : public IntervalTransformationTemplate<T> {
protected:
  T translate(const T &x) const override {
    return x;
  }
  T inverseTranslate(const T &x) const override {
    return x;
  }
public:
  bool isLinear() const override {
    return true;
  }
  LinearTransformationTemplate(const NumberInterval<T> &fromInterval, const NumberInterval<T> &toInterval)
    : IntervalTransformationTemplate<T>(fromInterval, toInterval) {
    computeTransformation();
  }
  IntervalScale getScale() const override {
    return LINEAR;
  }
  IntervalTransformationTemplate<T> *clone() const override {
    IntervalTransformationTemplate<T> *t = new LinearTransformationTemplate(*this); TRACE_NEW(t);
    return t;
  }
};

typedef LinearTransformationTemplate<float   > FloatLinearTransformation;
typedef LinearTransformationTemplate<double  > LinearTransformation;
typedef LinearTransformationTemplate<Double80> D80LinearTransformation;
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
  bool isLinear() const override {
    return false;
  }
  LogarithmicTransformationTemplate(const NumberInterval<T> &fromInterval, const NumberInterval<T> &toInterval)
    : IntervalTransformationTemplate<T>(fromInterval, toInterval) {
    computeTransformation();
  }
  IntervalScale getScale() const override {
    return LOGARITHMIC;
  }
  IntervalTransformationTemplate<T> *clone() const override {
    IntervalTransformationTemplate<T> *t = new LogarithmicTransformationTemplate(*this); TRACE_NEW(t);
    return t;
  }
};

typedef LogarithmicTransformationTemplate<float   > FloatLogarithmicTransformation;
typedef LogarithmicTransformationTemplate<double  > LogarithmicTransformation;
typedef LogarithmicTransformationTemplate<Double80> D80LogarithmicTransformation;
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
    return norm(x);
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
    : IntervalTransformationTemplate<T>(fromInterval, toInterval)
  {
    computeTransformation();
  }
  bool isLinear() const override {
    return false;
  }
  IntervalScale getScale() const override {
    return NORMAL_DISTRIBUTION;
  }
  IntervalTransformationTemplate<T> *clone() const override {
    IntervalTransformationTemplate<T> *t = new NormalDistributionTransformationTemplate(*this); TRACE_NEW(t);
    return t;
  }
};

typedef NormalDistributionTransformationTemplate<float   > FloatNormalDistributionTransformation;
typedef NormalDistributionTransformationTemplate<double  > NormalDistributionTransformation;
typedef NormalDistributionTransformationTemplate<Double80> D80NormalDistributionTransformation;
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
