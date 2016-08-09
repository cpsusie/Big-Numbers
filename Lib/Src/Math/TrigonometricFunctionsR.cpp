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


