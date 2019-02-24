#include "pch.h"
#include <Math/Real.h>

// Assume x >= 0
Real chiSquaredDensity(const Real &df, const Real &x) {
  DEFINEMETHODNAME;
  if (x < 0) {
    throwInvalidArgumentException(method, _T("x=%s. Valid range[0;inf["), toString(x).cstr());
  }
  const Real df2 = df / 2.0;
  return exp(-x/2.0) * pow(x, df2-1) / pow((Real)2.0,df2) / gamma(df2);
}

// Calculate lower incomplete gamma function
Real lowerIncGamma(const Real &a, const Real &x) {
  DEFINEMETHODNAME;
  if (x < 0) {
    throwInvalidArgumentException(method, _T("x=%s. Valid range:[0; inf["), toString(x).cstr());
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

/*
Real chiSquaredDistribution(UINT df, const Real &x) {
  const Real df2    = (Real)df/2.0;
  const Real g      = gamma(df2);
  return lowerIncGammaTaylor(df2,x/2.0) / g;
}
*/

// Calculate ln(lower incomplete gamma function)
static Real lnLowerIncGammaTaylor(const Real &a, const Real &x) {
  if(x <= 0) {
    throwInvalidArgumentException(__TFUNCTION__, _T("x=%s. Valid range:]0; inf["), toString(x).cstr());
  }
  Real sum = 0, ai = a, p = 1.0/ai++;

  for(;;) {
    Real lastSum = sum;
    sum += p;
    if(sum == lastSum) {
      break;
    }
    p *= (x / ai++);
    if(isinf(p)) {
      return p;
    }
  }
  return a * log(x) + log(sum) - x; // ln(exp(-x) * pow(x, a) * sum);
}

Real chiSquaredDistribution(const Real &df, const Real &x) {
  if (x <= 0) {
    return 0;
  }
  const Real df2    = (Real)df/2.0;
  const Real lnincG = lnLowerIncGammaTaylor(df2,x/2.0);
  if(isinf(lnincG)) {
    return 1.0;
  }
  const Real lnG    = lnGamma(df2);
  const Real diff   = lnincG - lnG;
  const Real result = (diff < -400) ? 0 : exp(diff);
  return minMax(result, Real(0.0), Real(1.0));
}
