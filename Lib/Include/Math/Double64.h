#pragma once

#include "PragmaLib.h"

int    getExpo10(         double x);

double mypow(             double x, double y);
double root(              double x, double y);
double cot(               double x);
double acot(              double x);

double gamma(             double x);
double lnGamma(           double x);
double fac(               double x);

// calculate both cos and sin. c:inout c, s:out
extern "C" {
  void   sincos(double &c, double &s);
  double exp10(double x);
};

#define FLT_NAN  std::numeric_limits<float>::quiet_NaN()
#define FLT_PINF std::numeric_limits<float>::infinity()
#define FLT_NINF (-FLT_PINF)
#define DBL_NAN  std::numeric_limits<double>::quiet_NaN()
#define DBL_PINF std::numeric_limits<double>::infinity()
#define DBL_NINF (-DBL_PINF)
