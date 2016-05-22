#include "pch.h"

Vector getGradient(VectorToRFunction &f, const Vector &x) {
  return getGradient(f,x,f(x));
}

#define EPSILON 1e-9

/* getGradient=f'(x), ie partial derivatives of f. Assume y0 = f(x0) */
Vector getGradient(VectorToRFunction &f, const Vector &x0, const Real &y0) {
  Vector x1(x0);
  Vector result(x1.getDimension());

  for(unsigned int i = 0; i < x1.getDimension(); i++) {
    Real save = x1[i];
    Real eps  = (save == 0) ? EPSILON : EPSILON*save;
    x1[i] += eps;
    Real y1 = f(x1);
    x1[i] = save;
    result(i) = (y1 - y0) / eps;
  }
  return result;
}

Vector getGradient1(VectorToRFunction &f, const Vector &x) {
  Vector x1(x);

  Vector result(x.getDimension());
  for(unsigned int i = 0; i < x.getDimension(); i++) {
    Real save = x[i];
    Real eps  = (save == 0) ? EPSILON : EPSILON*save;
    x1[i] += eps;
    Real y1 = f(x1);
    x1[i] = save;
    x1[i] -= eps;
    Real y2 = f(x1);
    x1[i] = save;
    result(i) = (y1 - y2) / 2 / eps;
  }
  return result;
}

