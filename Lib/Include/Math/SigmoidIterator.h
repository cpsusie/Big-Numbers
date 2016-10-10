#pragma once

#include <Math/Transformation.h>

class SigmoidIterator {
private:
  const UINT           m_steps;
  double               m_current;
  UINT                 m_stepCounter;
  LinearTransformation m_tr;
  static double sigmoid(double x);
public:
  SigmoidIterator(double from, double to, UINT steps);
  bool hasNext() const;
  double next();
  UINT getSteps() const {
    return m_steps;
  }
};
