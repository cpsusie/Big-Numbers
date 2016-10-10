#include "pch.h"
#include <Math/SigmoidIterator.h>

SigmoidIterator::SigmoidIterator(double from, double to, UINT steps) 
: m_steps(steps)
, m_tr(DoubleInterval(sigmoid(-1.0+1.0/steps), sigmoid(1.0-1.0/steps)), DoubleInterval(from, to))
{
  if(steps == 0) {
    throwInvalidArgumentException(_T("SigmoidIterator"),_T("steps=%d"), steps);
  }
  m_current     = -1 + 1.0/m_steps;
  m_stepCounter = 0;
}

double SigmoidIterator::sigmoid(double x) { // static
  return 1.0 / (1.0+exp(-3*x));
}

bool SigmoidIterator::hasNext() const {
  return m_stepCounter < m_steps;
}

double SigmoidIterator::next() {
  if(++m_stepCounter == m_steps) {
    return m_tr.getToInterval().getTo();
  } else {
    const double result = m_tr.forwardTransform(sigmoid(m_current));
    m_current += 2.0/m_steps;
    return result;
  }
}

