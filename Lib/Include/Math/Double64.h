#pragma once

int    getExpo10(         double x);

double mypow(             double x, double y);
double root(              double x, double y);
double cot(               double x);
double acot(              double x);
// calculate both cos and sin. c:inout c, s:out
extern "C" {
  void   sincos(double &c, double &s);
  double exp10(double x);
};
