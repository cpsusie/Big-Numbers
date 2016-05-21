#include "pch.h"
#include <Math.h>

static double pow10(int p) {
  switch(p) {
  case  0 : return 1;
  case -1 :
  case  1 : return 10;
  case -2 :
  case  2 : return 100;
  case -3 :
  case  3 : return 1000;
  case -4:
  case  4 : return 10000;
  case -5 :
  case  5 : return 100000;
  case -6 :
  case  6 : return 1000000;
  case -7 :
  case  7 : return 10000000;
  case -8 :
  case  8 : return 100000000;
  case -9 :
  case  9 : return 1000000000;
  case -10:
  case  10: return 10000000000L;
  default : return pow(10,fabs(p));
  }
}

/*
double round(double x) { // 5-rounding
  return x < 0 ? -floor(0.5-x) : floor(0.5+x);
}
*/

double round(double x, int dec) { // 5-rounding
  if(dec == 0) {
    return x < 0 ? -floor(0.5-x) : floor(0.5+x);
  } else {
    double p = pow10(dec);
    if(dec > 0) {
      return x < 0 ? -floor(0.5-x*p)/p : floor(0.5+x*p)/p;
    } else { // dec < 0
      return x < 0 ? -floor(0.5-x/p)*p : floor(0.5+x/p)*p;
    }
  }
}

double trunc(double x, int dec) {
  if(dec == 0) {
    return x < 0 ? -floor(-x) : floor(x);
  } else {
    double p = pow10(dec);
    if(dec > 0) {
      return x < 0 ? -floor(-x*p)/p : floor(x*p)/p;
    } else { // dec < 0
      return x < 0 ? -floor(-x/p)*p : floor(x/p)*p;
    }
  }
}
