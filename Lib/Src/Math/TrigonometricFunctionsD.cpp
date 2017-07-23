#include "pch.h"

Real sinDegrees(const Real &x) {
  return sin(x * M_PI / 180.0);
}

Real cosDegrees(const Real &x) {
  return cos(x * M_PI / 180.0);
}

Real tanDegrees(const Real &x) {
  return tan(x * M_PI / 180.0);
}

Real cotDegrees(const Real &x) {
  return cot(x * M_PI / 180.0);
}

Real asinDegrees(const Real &x) {
  return asin(x) / M_PI * 180.0;
}

Real acosDegrees(const Real &x) {
  return acos(x) / M_PI * 180.0;
}

Real atanDegrees(const Real &x) {
  return atan(x) / M_PI * 180.0;
}

Real atan2Degrees(const Real &y, const Real &x) {
  return atan2(y,x) / M_PI * 180.0;
}

Real acotDegrees(const Real &x) {
  return acot(x) / M_PI * 180.0;
}

Real cscDegrees(const Real &x) {
  return csc(x * M_PI / 180.0);
}

Real secDegrees(const Real &x) {
  return sec(x * M_PI / 180.0);
}

Real acscDegrees(const Real &x) {
  return acsc(x) / M_PI * 180.0;
}

Real asecDegrees(const Real &x) {
  return asec(x) / M_PI * 180.0;
}
