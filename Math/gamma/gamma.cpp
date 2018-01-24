// gamma.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Math.h>
#include <Math/BigReal.h>
#include "Gamma.h"

static double Lgamma1_2(double x) {
  BigReal currentSum;
  BigReal lastSum;
  BigReal fac(1);
  BigReal c1(e(1,-23));
  BigReal X(x);
  BigReal TR(42);
// TR er udregnet så integralet(t*exp(-t))dt fra TR til #inf er < 1e-16
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
  return getDouble(currentSum * pow(TR,X,c1));
}

double Lgamma(double x) {
  if(x > 2)
    return (x-1)*Lgamma(x-1);
  else if(x < 1)
    return Lgamma(x+1)/x;
  else if(x == 1 || x == 2)
    return 1;
  else
    return Lgamma1_2(x);
}

double Lfac(double x) {
  return Lgamma(x+1);
}

int findlimit(double k) {
  double x = -log(k);
  x += log(x);
//  printf("x:%le\n",x);
  double f;
  do {
    double e = exp(-x);
    f = (x+1)*e-k;
    double df = -x*e;
    x -= f / df;
//    printf("x:%le f:%le relerror:%le\n",x,f,fabs(f) / k );
  } while(fabs(f) / k > 1e-3);
  return (int)x+1;
}


static double ddx_gamma1_2(double x) {
  BigReal currentSum;
  BigReal lastSum;
  BigReal fac = BIGREAL_1;
  BigReal c1  = e(BIGREAL_1,-23);
  BigReal X   = x;
  BigReal TR  = 42;
// TR er udregnet så integralet(t*exp(-t))dt fra TR til #inf er < 1e-16
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
  return getDouble(currentSum * pow(TR,X,c1));
}
