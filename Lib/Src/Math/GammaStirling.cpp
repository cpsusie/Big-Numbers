#include "pch.h"

// see https://en.wikipedia.org/wiki/Stirling%27s_approximation

Real gammaStirling(const Real &x) {
  return sqrt(2.0*M_PI/x) * pow((x + 1.0/(12.0*x - 1.0/(10.0*x)))/M_E,x);
}

Real lnGammaStirling(const Real &x) {
  if(x <= 0) {
    throwInvalidArgumentException(__TFUNCTION__,_T("x=%s. must be > 0"), toString(x).cstr()); // undefined
  }
  return log(2.0*M_PI/x)/2.0 + x * (log(x + 1.0/(12.0*x - 1.0/(10.0*x))) - 1.0);
}