#include "pch.h"

static const Real G2RFactor(REAL_PI / 200);
static const Real R2GFactor(200 / REAL_PI);

Real sinGrads(const Real &x) {
  return sin(x * G2RFactor);
}

Real cosGrads(const Real &x) {
  return cos(x * G2RFactor);
}

Real tanGrads(const Real &x) {
  return tan(x * G2RFactor);
}

Real cotGrads(const Real &x) {
  return cot(x * G2RFactor);
}

Real asinGrads(const Real &x) {
  return asin(x) * R2GFactor;
}

Real acosGrads(const Real &x) {
  return acos(x) * R2GFactor;
}

Real atanGrads(const Real &x) {
  return atan(x) * R2GFactor;
}

Real atan2Grads(const Real &y, const Real &x) {
  return atan2(y,x) * R2GFactor;
}

Real acotGrads(const Real &x) {
  return acot(x) * R2GFactor;
}

Real cscGrads(const Real &x) {
  return csc(x * G2RFactor);
}

Real secGrads(const Real &x) {
  return sec(x * G2RFactor);
}

Real acscGrads(const Real &x) {
  return acsc(x) * R2GFactor;
}

Real asecGrads(const Real &x) {
  return asec(x) * R2GFactor;
}

