#pragma once

#include "Matrix.h"

class RungeKuttaFehlbergHandler;

class RungeKuttaFehlberg {
private:
  VectorFunction            &m_diff;    // calculates f'(y), y[0] = x, y[1..dim] = y, so actually f(x,y)
  RungeKuttaFehlbergHandler &m_handler; // called everytime m_y is set
  Vector                     m_y;
  void setValue(const Vector &v);
public:
  RungeKuttaFehlberg(VectorFunction &diff, RungeKuttaFehlbergHandler &handler);
  const Vector &calculate(const Vector &start, Real v0end, Real maxError);
  const Vector &getvalue() const {
    return m_y;
  }
  VectorFunction &getFunction() const {
    return m_diff;
  }
};

class RungeKuttaFehlbergHandler : public AlgorithmHandler<RungeKuttaFehlberg> {
};

void rungeKuttaStep(Vector &v, Real step, VectorFunction &diff);
