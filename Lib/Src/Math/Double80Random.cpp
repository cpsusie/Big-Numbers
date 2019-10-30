#include "pch.h"
#include <Math/Double80.h>

Double80 randDouble80(RandomGenerator &rnd) {
  static const Double80 q = (UINT64)(-1);
  Double80 tmp  = randInt64(rnd);
  return tmp / q;
}

Double80 randDouble80(const Double80 &low, const Double80 &high, RandomGenerator &rnd) {
  return (high-low)*randDouble80(rnd)+low;
}

static const Double80 _2PI(DBL80_PI * 2);
Double80 randGaussianD80(const Double80 &mean, const Double80 &s, RandomGenerator &rnd) {
  const Double80 u1 = randDouble80(rnd);
  Double80 u2;
  while((u2 = randDouble80(rnd)).isZero()); // u2 != 0
  return mean + s * cos(_2PI * u1) * sqrt(-2 * log(u2));
}
