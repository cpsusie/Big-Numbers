#include "pch.h"

#define _norm_factor  0.3989422804014327 /* 1/sqrt(2*PI) */

Real gauss(const Real &x) {
  return(_norm_factor*exp(-x*x/2));
}

Real norm(const Real &x) {
  static const Real a[] = {
    0,
    0.31938153,
   -0.356563782,
    1.781477937,
   -1.821255978,
    1.330274429
  };

  bool neg = false;

  if(x < -10) return 0.0;
  if(x > 10)  return 1.0;

  Real X;
  if(x < 0) {
    neg = true;
    X = -x;
  } else {
    X = x;
  }
  Real t = 1/(1+0.2316419*X);
  t = gauss(X) * poly(t, 5, a);

  return neg ? t : (1-t);
}


// inverse of norm(x). That is: probitFunction(norm(x)) = norm(probitFunction(x)) = x
Real probitFunction(const Real &x) {
  static const Real a[]   = {-3.969683028665376e+01
                            , 2.209460984245205e+02
                            ,-2.759285104469687e+02
                            , 1.383577518672690e+02
                            ,-3.066479806614716e+01
                            , 2.506628277459239e+00
  };

  static const Real b[]   = {-5.447609879822406e+01
                            , 1.615858368580409e+02
                            ,-1.556989798598866e+02
                            , 6.680131188771972e+01
                            ,-1.328068155288572e+01
                            ,1
  };

  static const Real c[]   = {-7.784894002430293e-03
                            ,-3.223964580411365e-01
                            ,-2.400758277161838e+00
                            ,-2.549732539343734e+00
                            , 4.374664141464968e+00
                            , 2.938163982698783e+00
  };

  static const Real d[]   = {7.784695709041462e-03
                            ,3.224671290700398e-01
                            ,2.445134137142996e+00
                            ,3.754408661907416e+00
                            ,1
  };

  static const Real p_low  = 0.02425;
  static const Real p_high = 1 - p_low;

  if (0 < x && x < p_low) {
    Real q = sqrt(-2 * log(x));
    return poly1(q,5,c) / poly1(q,4,d);
  }

  if(p_low <= x && x <= p_high) {
    Real q = x - 0.5;
    Real r = q*q;
    return q*poly1(r,5,a) / poly1(r,5,b);
  }

  if(p_high < x && x < 1) {
    Real q = sqrt(-2 * log(1-x));
    return -poly1(q,5,c) / poly1(q,4,d);
  }
  return sqrt(-1); // nan
}

Real errorFunction(const Real &x) {
  return 2 * norm(x * sqrt(2)) - 1;
}


// inverse of errorFunction. ie. inverseErrorFunction(errorFunction(x)) = x
Real inverseErrorFunction(const Real &x) {
  return probitFunction((x+1)/2) / sqrt(2);
}
