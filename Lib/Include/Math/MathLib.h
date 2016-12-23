#pragma once

#include "MathFunctions.h"

template <class Domain, class Range> class FunctionTemplate {
public:
  virtual Range operator()(const Domain &x) = 0;
};

#include "Complex.h"
#include "Matrix.h"

typedef FunctionTemplate<Real,Real>         Function;
typedef FunctionTemplate<Vector,Vector>     VectorFunction;
typedef FunctionTemplate<Vector,Real>       VectorToRFunction;
typedef FunctionTemplate<Real,Vector>       RToVectorFunction;
typedef FunctionTemplate<Complex,Complex>   ComplexFunction;

Vector getGradient( VectorToRFunction &f, const Vector &x);
Vector getGradient1(VectorToRFunction &f, const Vector &x);
Vector getGradient( VectorToRFunction &f, const Vector &x0, const Real &y0);
Matrix getJacobi(   VectorFunction    &f, const Vector &x);
Matrix getJacobi(   VectorFunction    &f, const Vector &x0, const Vector &y0);

void rungeKuttaStep(Vector &v, Real step, VectorFunction &diff);
Vector newton(      VectorFunction &f, const Vector &x0, int maxit = 10, Real tolerance = 1e-10);
Vector davidenko(   VectorFunction &f, const Vector &x0);
Vector levenberg(   VectorFunction &f, const Vector &x0);

class RungeKuttaFehlbergHandler;

class RungeKuttaFehlberg {
private:
  VectorFunction            &m_diff;    // calculates f'(y), y[0] = x, y[1..dim] = y, so actually f(x,y)
  RungeKuttaFehlbergHandler &m_handler; // called everytime m_y is set
  Vector                     m_y;
  void setValue(const Vector &v);
public:
  RungeKuttaFehlberg(VectorFunction &diff, RungeKuttaFehlbergHandler &handler);
  const Vector &calculate(const Vector &start, Real v0end, Real eps);
  const Vector &getvalue() const {
    return m_y;
  }
  VectorFunction &getFunction() const {
    return m_diff;
  }
};

class RungeKuttaFehlbergHandler : public AlgorithmHandler<RungeKuttaFehlberg> {
};

ComplexVector roots(const Complex &c, int r);

#define verifyEqualsInt(   expected, value)            verify(value == expected)
#define verifyAlmostEquals(expected, value,tolerance)  verify(fabs((expected)-(value)) <= (tolerance))

