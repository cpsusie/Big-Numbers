#include "pch.h"
#include <Math/Expression/ExpressionFunctionR2R3.h>

namespace Expr {

ExpressionFunctionR2R3::ExpressionFunctionR2R3(const Expr3 &expr, TrigonometricMode mode, bool machineCode, const String &names) {
  compile(expr, mode, machineCode, names);
}

void ExpressionFunctionR2R3::compile(const Expr3 &expr, TrigonometricMode mode, bool machineCode, const String &names) {
  for(int i = 0; i < ARRAYSIZE(m_expr); i++) {
    m_expr[i].compile(expr.getExprText(i), mode, machineCode, names);
  }
}

Point3D ExpressionFunctionR2R3::operator()(const Real &t, const Real &s) {
  return Point3D(m_expr[0](t,s), m_expr[1](t,s), m_expr[2](t,s));
}

Point3D ExpressionFunctionR2R3::operator()(const Point2D &p) {
  return Point3D(m_expr[0](p), m_expr[1](p), m_expr[2](p));
}

void ExpressionFunctionR2R3::setTime(const Real &time) {
  for(int i = 0; i < ARRAYSIZE(m_expr); i++) {
    m_expr[i].setTime(time);
  }
}

const Real &ExpressionFunctionR2R3::getTime() const {
  return m_expr[0].getTime();
}

FunctionWithTimeTemplate<FunctionR2R3> *ExpressionFunctionR2R3::clone() const {
  return new ExpressionFunctionR2R3(*this);
}

}; // namespace Expr
