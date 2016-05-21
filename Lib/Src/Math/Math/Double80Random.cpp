#include "pch.h"
#include <Math.h>
#include <Math/Double80.h>

Double80 RandomD80::nextDouble80() {
  static const Double80 q = (unsigned __int64)(-1);
  Double80 tmp  = (unsigned __int64)nextInt64();
  return tmp / q;
}

Double80 RandomD80::nextDouble80(const Double80 &low, const Double80 &high) {
  return (high-low)*nextDouble80()+low;
}
