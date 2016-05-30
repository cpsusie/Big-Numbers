#include "pch.h"
#include <Math/Matrix.h>

// superdiag = (c0=a(0,1),c1=a(1,2),c2=a(2,3)...,0)
// subdiag   = (0,c1=a(1,0),c2=a(2,1),c3=a(3,2)...)
Vector solve3diag(const Vector &superdiag, const Vector &diag, const Vector &subdiag, const Vector &d) {
  int n = d.getDimension();
  Vector a(n);
  Vector b(n);
  b[0] = diag[0];
  for(int k = 1; k < n; k++) {
    a[k] = subdiag[k] / b[k-1];
    b[k] = diag[k] - a[k] * superdiag[k-1];
  }

  Vector delta(n);

  delta[0] = d[0];
  for(int k = 1; k < n; k++) {
    delta[k] = d[k] - a[k] * delta[k - 1];
  }

  Vector x(n);
  x[n-1] = delta[n-1] / b[n-1];
  for(int k = n - 2; k >= 0; k--) {
    x[k] = (delta[k] - superdiag[k] * x[k + 1]) / b[k];
  }

  return x;
}
