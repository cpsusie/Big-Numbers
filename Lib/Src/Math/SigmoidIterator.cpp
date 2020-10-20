#include "pch.h"
#include <Math/IntervalTransformation.h>
#include <Math/SigmoidIterator.h>

class _SigmoidIterator : public AbstractIterator {
private:
  DECLARECLASSNAME;
  const LinearTransformation m_tr;
  const UINT                 m_steps;
  double                     m_current, m_iteratorValue;
  UINT                       m_stepCounter;
  static double sigmoid(double x);
  static inline double startValue(UINT steps) {
    return -endValue(steps);
  }
  static inline double endValue(UINT steps) {
    return 1.0 - 1.0 / steps;
  }
public:
  _SigmoidIterator(const DoubleInterval &interval, UINT steps);
  AbstractIterator     *clone()             override {
    return new _SigmoidIterator(*this);
  }
  bool                  hasNext()     const override {
    return m_stepCounter < m_steps;
  }
  void                 *next()              override;
  void                  remove()            override {
    unsupportedOperationError(__TFUNCTION__);
  }
  UINT                  getSteps()    const {
    return m_steps;
  }
  const DoubleInterval &getInterval() const {
    return m_tr.getToInterval();
  }
};

DEFINECLASSNAME(_SigmoidIterator);

_SigmoidIterator::_SigmoidIterator(const DoubleInterval &interval, UINT steps)
: m_tr(DoubleInterval(sigmoid(startValue(steps)), sigmoid(endValue(steps))), interval)
, m_steps(steps)
{
  if(steps == 0) {
    throwInvalidArgumentException(__TFUNCTION__, _T("steps=%d"), steps);
  }
  m_current     = startValue(steps);
  m_stepCounter = 0;
}

double _SigmoidIterator::sigmoid(double x) { // static
  return 1.0 / (1.0 + exp(-3 * x));
}

void *_SigmoidIterator::next() {
  if(!hasNext()) {
    noNextElementError(s_className);
  }
  if(++m_stepCounter == m_steps) {
    m_iteratorValue = m_tr.getToInterval().getTo();
  } else {
    m_iteratorValue = m_tr.forwardTransform(sigmoid(m_current));
    m_current += 2.0 / m_steps;
  }
  return &m_iteratorValue;
}

SigmoidIterator::SigmoidIterator(const DoubleInterval &interval, UINT steps)
: Iterator<double>(new _SigmoidIterator(interval, steps))
{
}

SigmoidIterator::SigmoidIterator(double from, double to, UINT steps)
: Iterator<double>(new _SigmoidIterator(DoubleInterval(from, to), steps))
{
}

const DoubleInterval &SigmoidIterator::getinterval() const {
  return ((_SigmoidIterator*)m_it)->getInterval();
}

UINT SigmoidIterator::getSteps() const {
  return ((_SigmoidIterator*)m_it)->getSteps();
}
