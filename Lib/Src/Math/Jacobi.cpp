#include "pch.h"
#include <Math/Matrix.h>

Matrix getJacobi(VectorFunction &f, const Vector &x) {
  return getJacobi(f,x,f(x));
}

#define EPSILON 1e-7

/* getJacobi=f'(x0) Assume y0 = f(x0) */
Matrix getJacobi(VectorFunction &f, const Vector &x0, const Vector &y0) {
  Vector x1(x0);
  Matrix result(y0.getDimension(),x0.getDimension());

  for(UINT i = 0; i < x1.getDimension(); i++) {
    Real &xi = x1[i];
    Real save = xi;
    Real eps  = (save == 0) ? EPSILON : EPSILON * save;
    xi += eps;
    result.setColumn(i,(f(x1) - y0)/eps);
    xi = save;
  }
  return result;
}
