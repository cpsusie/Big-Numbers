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
double round(double v) { // 5-rounding
  return v < 0 ? -floor(0.5-v) : floor(0.5+v);
}
*/

double round(double v, int dec) {
  if(dec == 0) {
    return v < 0 ? -floor(0.5-v) : floor(0.5+v);
  } else {
    double p = pow10(dec);
    if(dec > 0) {
      return v < 0 ? -floor(0.5-v*p)/p : floor(0.5+v*p)/p;
    } else { // dec < 0
      return v < 0 ? -floor(0.5-v/p)*p : floor(0.5+v/p)*p;
    }
  }
}

double trunc(double v, int dec) {
  if(dec == 0) {
    return v < 0 ? -floor(-v) : floor(v);
  } else {
    double p = pow10(dec);
    if(dec > 0) {
      return v < 0 ? -floor(-v*p)/p : floor(v*p)/p;
    } else { // dec < 0
      return v < 0 ? -floor(-v/p)*p : floor(v/p)*p;
    }
  }
}
