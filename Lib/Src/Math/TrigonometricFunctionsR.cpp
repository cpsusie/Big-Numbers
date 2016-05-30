#include "pch.h"

double cot(double x) {
  return 1.0/tan(x);
}

double acot(double x) {
  return M_PI/2 - atan(x);
}

Real csc(const Real &x) {
  return 1.0/sin(x);
}

Real sec(const Real &x) {
  return 1.0/cos(x);
}

Real acsc(const Real &x) {
  return asin(1.0/x);
}

Real asec(const Real &x) {
  return acos(1.0/x);
}

Real acosh(const Real &x) {
  return log(x + sqrt(x*x-1));
}

Real asinh(const Real &x) {
  return log(x + sqrt(x*x+1));
}

Real atanh(const Real &x) {
  return log(sqrt((1+x)/(1-x)));
}


