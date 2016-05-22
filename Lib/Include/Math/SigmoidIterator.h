#pragma once

#include <Math/Transformation.h>

class SigmoidIterator {
private:
  const unsigned int   m_steps;
  double               m_current;
  unsigned int         m_stepCounter;
  LinearTransformation m_tr;
  static double sigmoid(double x);
public:
  SigmoidIterator(double from, double to, unsigned int steps);
  bool hasNext() const;
  double next();
  unsigned int getSteps() const {
    return m_steps;
  }
};
