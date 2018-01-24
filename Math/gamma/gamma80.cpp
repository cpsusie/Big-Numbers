// gamma.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Math.h>
#include <Math/BigReal.h>
#include "Gamma.h"

static Double80 Lgamma1_2(const Double80 &x) {
  BigReal currentSum;
  BigReal lastSum;
  BigReal fac = BIGREAL_1;
  BigReal c1  = e(BIGREAL_1,-23);
  BigReal X   = x;
  BigReal TR  = 53;

// TR er udregnet så integralet(t*exp(-t))dt fra TR til #inf er < 1e-21
  BigReal p(1);
  for(int i = 0;;i++) {
    lastSum = currentSum;
    BigReal term = quot(quot(p,fac,c1),X+i,c1);
//    BigReal tt1(x+i);
//    BigReal tt2(X+BigReal(i));
//printf("tt1:"); tt1.print(); printf(" tt2:"); tt2.print(); printf("\n");
    currentSum = sum(currentSum,term,c1);
    if(currentSum == lastSum) {
      break;
    }
    p = p * TR;
    fac = -fac * (i+1);
  }
  return getDouble80(currentSum * pow(TR,X,c1));
}

Double80 Lgamma80(const Double80 &x) {
  if(x > 2)
    return (x-1)*Lgamma80(x-1);
  else if(x < 1)
    return Lgamma80(x+1)/x;
  else if(x == 1 || x == 2)
    return 1;
  else
    return Lgamma1_2(x);
}

Double80 Lfac80(const Double80 &x) {
  return Lgamma80(x+1);
}
