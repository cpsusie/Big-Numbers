#include "stdafx.h"
#include <stdio.h>
#include <math.h>

int main(int argc, char **argv) {
  double y[30];
  double a[30];

  y[0] = sqrt(2.0) - 1;
  a[0] = 6 - 4*sqrt(2.0);
  double pow2 = 8;
  for(int i = 1; i < 20; i++) {
    double tmp = pow(1.0 - pow(y[i-1],4), 0.25);
    y[i] = (1-tmp) / (1+tmp);
    a[i] = pow(1+y[i],4) * a[i-1] - pow2 * y[i] * (1 + y[i] + y[i]*y[i]);
    printf("%15.20lf %15.20lf\n",y[i],1.0/a[i]);
    pow2 *= 4;
  }
  return 0;
}
