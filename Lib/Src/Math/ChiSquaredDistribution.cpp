#include "pch.h"
#include <Math.h>
#include <Math/MathFunctions.h>

// see http://www.geosci-model-dev.net/3/329/2010/gmd-3-329-2010.pdf
// Efficient approximation of the incomplete gamma function for use
// in cloud model applications

#ifdef __NEVER__
static const double p[] = {
  9.4368392235e-03
,-1.0782666481e-04
,-5.8969657295e-06
, 2.8939523781e-07
, 1.0043326298e-01
, 5.5637848465e-01
};

static const double q[] = {
  1.1464706419e-01
, 2.6963429121e+00
,-2.9647038257e+00
, 2.1080724954e+00
};

static const double r[] = {
  0.0
, 1.1428716184e+00
,-6.6981186438e-03
, 1.0480765092e-04
};

static const double s[] = {
  1.0356711153e+00
, 2.3423452308e+00
,-3.6174503174e-01
,-3.1376557650e+00
, 2.9092306039e+00
};

static double C1(double a) {
  return (((p[3] * a + p[2]) * a + p[1]) * a + p[0]) * a + 1 + p[4] * (exp(-a * p[5]) - 1);
}

static double C2(double a) {
  double sum = q[0];
  double d   = a;
  for (int i = 1; i < 4; i++) {
    sum += q[i] / d;
    d *= a;
  }
  return sum;
}

static double C3(double a) {
  return ((r[3] * a + r[2]) * a + r[1]) * a;
}

static double C4(double a) {
  double sum = s[0];
  double d   = a;
  for (int i = 1; i < 5; i++) {
    sum += s[i] / d;
    d *= a;
  }
  return sum;
}

static double W(double a, double x) {
  return 0.5 * (1.0 + tanh(C2(a)*(x-C3(a))));
}

// Calculate lower incomplete gamma function
// Assume:0.9 <= a <= 45 and x >= 0
// and if ga!=NULL, then *ga=gamma(a)
static double lowerIncGamma(double a, double x, const double *ga = NULL) {
  if (a < 0.9 && a > 45) {
    throwInvalidArgumentException(__TFUNCTION__, _T("a=%le. Valid range:[0.9; 45]"), a);
  }
  if (x < 0) {
    throwInvalidArgumentException(__TFUNCTION__, _T("x=%le. Valid range:[0; inf["), x);
  }
  const double c1x = C1(a) * x;
  double       d   = 1.0/a;
  double       sum = 0;
  for(int i = 1; i < 3; i++) {
    sum += d;
    d *= (c1x / (a+i));
  }
  const double approxNear0   = exp(-x) * pow(x,a) * sum;
  const double approxNearInf = (ga?*ga:gamma(a)) * (1.0 - pow(C4(a),-x));
  const double w             = W(a, x);
  return  (1.0 - w) * approxNear0 + w * approxNearInf;
}

double chiSquaredDistribution(UINT df, double x) {
  const double df2 = (double)df/2;
  const double g   = gamma(df2);
  return lowerIncGamma(df2,x/2,&g) / g;
}

#endif // __NEVER__


// Assume x >= 0
Real chiSquaredDensity(UINT df, const Real &x) {
  if (x < 0) {
    throwInvalidArgumentException(__TFUNCTION__, _T("x=%s. Valid range[0;inf["), toString(x).cstr());
  }
  const Real df2 = (Real)df / 2.0;
  return exp(-x/2.0) * pow(x, df2-1) / pow((Real)2.0,df2) / gamma(df2);
}

/*
// Calculate lower incomplete gamma function
static Real lowerIncGammaTaylor(const Real &a, const Real &x) {
  if (x < 0) {
    throwInvalidArgumentException(__TFUNCTION__, _T("x=%s. Valid range:[0; inf["), toString(x).cstr());
  }
  Real sum = 0, ai = a, p = 1.0/ai++;

  for(;;) {
    Real lastSum = sum;
    sum += p;
    if(sum == lastSum) {
      break;
    }
    p *= (x / ai++);
  }
  return exp(-x) * pow(x, a) * sum;
}

Real chiSquaredDistribution(UINT df, const Real &x) {
  const Real df2    = (Real)df/2.0;
  const Real g      = gamma(df2);
  return lowerIncGammaTaylor(df2,x/2.0) / g;
}
*/

// Calculate ln(lower incomplete gamma function)
static Real lnLowerIncGammaTaylor(const Real &a, const Real &x) {
  if(x <= 0) {
    throwInvalidArgumentException(__TFUNCTION__, _T("x=%s. Valid range:[0; inf["), toString(x).cstr());
  }
  Real sum = 0, ai = a, p = 1.0/ai++;

  for(;;) {
    Real lastSum = sum;
    sum += p;
    if(sum == lastSum) {
      break;
    }
    p *= (x / ai++);
    if(isInfinity(p)) {
      return p;
    }
  }
  return a * log(x) + log(sum) - x; // ln(exp(-x) * pow(x, a) * sum);
}

Real chiSquaredDistribution(UINT df, const Real &x) {
  if (x <= 0) {
    return 0;
  }
  const Real df2    = (Real)df/2.0;
  const Real lnincG = lnLowerIncGammaTaylor(df2,x/2.0);
  if(isInfinity(lnincG)) {
    return 1.0;
  }
  const Real lnG    = lnGamma(df2);
  const Real diff   = lnincG - lnG;
  const Real result = (diff < -400) ? 0 : exp(diff);
  return minMax(result, 0.0, 1.0);
}
