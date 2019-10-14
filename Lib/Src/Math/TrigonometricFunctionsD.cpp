#include "pch.h"

static const Real D2RFactor(REAL_PI / 180);
static const Real R2DFactor(180 / REAL_PI);

Real sinDegrees(const Real &x) {
  return sin(x * D2RFactor);
}

Real cosDegrees(const Real &x) {
  return cos(x * D2RFactor);
}

Real tanDegrees(const Real &x) {
  return tan(x * D2RFactor);
}

Real cotDegrees(const Real &x) {
  return cot(x * D2RFactor);
}

Real asinDegrees(const Real &x) {
  return asin(x) * R2DFactor;
}

Real acosDegrees(const Real &x) {
  return acos(x) * R2DFactor;
}

Real atanDegrees(const Real &x) {
  return atan(x) * R2DFactor;
}

Real atan2Degrees(const Real &y, const Real &x) {
  return atan2(y,x) * R2DFactor;
}

Real acotDegrees(const Real &x) {
  return acot(x) * R2DFactor;
}

Real cscDegrees(const Real &x) {
  return csc(x * D2RFactor);
}

Real secDegrees(const Real &x) {
  return sec(x * D2RFactor);
}

Real acscDegrees(const Real &x) {
  return acsc(x) * R2DFactor;
}

Real asecDegrees(const Real &x) {
  return asec(x) * R2DFactor;
}
