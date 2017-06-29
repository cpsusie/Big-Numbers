#include "pch.h"
#include <Math/Double80.h>

Double80 randDouble80(Random *rnd) {
  static const Double80 q = (UINT64)(-1);
  Double80 tmp  = rnd ? (UINT64)rnd->nextInt64() : randInt64();
  return tmp / q;
}

Double80 randDouble80(const Double80 &low, const Double80 &high, Random *rnd) {
  return (high-low)*randDouble80(rnd)+low;
}
