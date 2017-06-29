#include "pch.h"
#include <Math/Double80.h>

Double80 randDouble80() {
  static const Double80 q = (UINT64)(-1);
  Double80 tmp  = randInt64();
  return tmp / q;
}

Double80 randDouble80(const Double80 &low, const Double80 &high) {
  return (high-low)*randDouble80()+low;
}
