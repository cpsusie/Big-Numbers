#include "pch.h"
#include "Math/Transformation.h"
#include <Math/SigmoidIterator.h>

class _SigmoidIterator : public AbstractIterator {
private:
  const UINT           m_steps;
  double               m_current, m_iteratorValue;
  UINT                 m_stepCounter;
  LinearTransformation m_tr;
  static double sigmoid(double x);
public:
  _SigmoidIterator(double from, double to, UINT steps);
  AbstractIterator *clone() {
    return new _SigmoidIterator(*this);
  }
  bool hasNext() const;
  void *next();
  void remove();
  UINT getSteps() const {
    return m_steps;
  }
};

_SigmoidIterator::_SigmoidIterator(double from, double to, UINT steps)
: m_steps(steps)
, m_tr(DoubleInterval(sigmoid(-1.0 + 1.0 / steps), sigmoid(1.0 - 1.0 / steps)), DoubleInterval(from, to))
{
  if(steps == 0) {
    throwInvalidArgumentException(__TFUNCTION__, _T("steps=%d"), steps);
  }
  m_current = -1 + 1.0 / m_steps;
  m_stepCounter = 0;
}

double _SigmoidIterator::sigmoid(double x) { // static
  return 1.0 / (1.0 + exp(-3 * x));
}

bool _SigmoidIterator::hasNext() const {
  return m_stepCounter < m_steps;
}

void *_SigmoidIterator::next() {
  if(++m_stepCounter == m_steps) {
    m_iteratorValue = m_tr.getToInterval().getTo();
  } else {
    m_iteratorValue = m_tr.forwardTransform(sigmoid(m_current));
    m_current += 2.0 / m_steps;
  }
  return &m_iteratorValue;
}

void _SigmoidIterator::remove() {
  unsupportedOperationError(__TFUNCTION__);
}

SigmoidIterator::SigmoidIterator(double from, double to, UINT steps)
: Iterator<double>(new _SigmoidIterator(from, to, steps))
{
}

UINT SigmoidIterator::getSteps() const {
  return ((_SigmoidIterator*)m_it)->getSteps();
}
