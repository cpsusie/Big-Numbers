#include "pch.h"
#include <Math/Matrix.h>

void setToRandom(ComplexVector &v, RandomGenerator &rnd) {
  for(size_t i = 0; i < v.getDimension(); i++) {
    setToRandom(v[i], rnd);
  }
}

Real fabs(const ComplexVector &v) {
  Real sum = 0;
  for(size_t i = 0; i < v.getDimension(); i++) {
    sum += arg2(v[i]);
  }
  return sqrt(sum);
}
