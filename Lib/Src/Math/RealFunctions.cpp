#include "pch.h"
#include <Limits.h>
#include <Float.h>
#include <Random.h>

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
