#include "stdafx.h"
#include <Math/FPU.h>
#include <Math/Double80.h>

Double80 relativeError(double x64, const Double80 &x80) {
  return x80.isZero() ? x64 : fabs(x80-x64)/x80;
}

String D80ToDbgString(const Double80 &d80) {
  const int expo10 = Double80::getExpo10(d80);
  if(expo10 < -4 || (expo10 >= 18)) {
    TCHAR str[50];
    return d80tot(str, d80);
  } else {
    return toString(d80, 19-expo10,21,ios::fixed);
  }
}

int main() {
  const Double80 f80(1.01324);
  const double   f64(1.01324);
  double x64 = 1e-200;

  FPU::setRoundMode(FPU_ROUNDCONTROL_ROUND);
  for(Double80 x80 = Double80::pow10(-200); x80 < 1e200; x80 *= f80, x64 *= f64) {
    String s = D80ToDbgString(x80);
//    const Double80 err = relativeError(x64,x80);
//    printf("%.16le\n", getDouble(err));
  }
  return 0;
}

