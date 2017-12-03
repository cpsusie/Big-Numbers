#include "pch.h"
#include <Limits.h>
#include <Float.h>
#include <Random.h>

Real binomial(const Real &n, const Real &k) {
  return fac(n)/(fac(k)*fac(n-k));
}

Real randReal() {
#ifndef LONGDOUBLE
  return randDouble();
#else
  return randDouble80();
#endif // LONGDOUBLE
}

Real randReal(const Real &lower, const Real &upper) {
  return randReal() * (upper-lower) + lower;
}

Real randomGaussian(const Real &mean, const Real &s) {
  return _standardRandomGenerator->nextGaussian(getDouble(mean),getDouble(s));
}

void setToRandom(Real &r) {
  r = randReal();
}

Real poly(const Real &x, int degree, const Real *coef) {
  const Real *cp = coef + degree;
  Real result = *(cp--);
  while(cp >= coef) {
    result = result * x + *(cp--);
  }
  return result;
}

Real poly1(const Real &x, int degree, const Real *coef) {
  Real result = *coef;
  for(const Real *last = coef + degree; coef++ < last;) {
    result = result * x + *coef;
  }
  return result;
}
