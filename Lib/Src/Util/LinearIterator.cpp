#include "pch.h"
#include <LinearIterator.h>

class _LinearIterator : public AbstractIterator {
private:
  const DoubleInterval m_interval;
  const UINT           m_steps;
  UINT                 m_stepCounter;
  const double         m_step;
  double               m_iteratorValue, m_nextValue;
public:
  _LinearIterator(const DoubleInterval &interval, UINT steps);
  AbstractIterator *clone()         override {
    return new _LinearIterator(*this);
  }
  bool              hasNext() const override {
    return m_stepCounter < m_steps;
  }
  void             *next()          override;
  void              remove()        override {
    unsupportedOperationError(__TFUNCTION__);
  }
  inline const DoubleInterval &getInterval() const {
    return m_interval;
  }
  inline UINT getSteps() const {
    return m_steps;
  }
};

_LinearIterator::_LinearIterator(const DoubleInterval &interval, UINT steps)
: m_interval(interval)
, m_steps(steps)
, m_step((steps<=1) ? 0 : (interval.getLength()/(steps-1.0)))
, m_stepCounter(0)
, m_iteratorValue(interval.getFrom())
, m_nextValue(interval.getFrom())
{
}

void *_LinearIterator::next() {
  if(!hasNext()) {
    noNextElementError(__TFUNCTION__);
  }
  if(++m_stepCounter >= m_steps) {
    m_iteratorValue = m_interval.getTo();
  } else {
    m_iteratorValue = m_nextValue;
    m_nextValue += m_step;
  }
  return &m_iteratorValue;
}

LinearIterator::LinearIterator(const DoubleInterval &interval, UINT steps)
: Iterator<double>(new _LinearIterator(interval, steps))
{
}

LinearIterator::LinearIterator(double from, double to, UINT steps)
: Iterator<double>(new _LinearIterator(DoubleInterval(from, to), steps))
{
}

const DoubleInterval &LinearIterator::getinterval() const {
  return ((_LinearIterator*)m_it)->getInterval();
}

UINT LinearIterator::getSteps() const {
  return ((_LinearIterator*)m_it)->getSteps();
}
