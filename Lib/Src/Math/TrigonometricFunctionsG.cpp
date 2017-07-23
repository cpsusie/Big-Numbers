#include "pch.h"

Real sinGrads(const Real &x) {
  return sin(x * M_PI / 200.0);
}

Real cosGrads(const Real &x) {
  return cos(x * M_PI / 200.0);
}

Real tanGrads(const Real &x) {
  return tan(x * M_PI / 200.0);
}

Real cotGrads(const Real &x) {
  return cot(x * M_PI / 200.0);
}

Real asinGrads(const Real &x) {
  return asin(x) / M_PI * 200.0;
}

Real acosGrads(const Real &x) {
  return acos(x) / M_PI * 200.0;
}

Real atanGrads(const Real &x) {
  return atan(x) / M_PI * 200.0;
}

Real atan2Grads(const Real &y, const Real &x) {
  return atan2(y,x) / M_PI * 200.0;
}

Real acotGrads(const Real &x) {
  return acot(x) / M_PI * 200.0;
}

Real cscGrads(const Real &x) {
  return csc(x * M_PI / 200.0);
}

Real secGrads(const Real &x) {
  return sec(x * M_PI / 200.0);
}

Real acscGrads(const Real &x) {
  return acsc(x) / M_PI * 200.0;
}

Real asecGrads(const Real &x) {
  return asec(x) / M_PI * 200.0;
}

