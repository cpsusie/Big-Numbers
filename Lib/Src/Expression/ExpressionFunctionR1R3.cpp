#include "pch.h"
#include <Math/Expression/ExpressionFunctionR1R3.h>

namespace Expr {

ExpressionFunctionR1R3::ExpressionFunctionR1R3(const Expr3 &expr, TrigonometricMode mode, bool machineCode, const String &names) {
  compile(expr, mode, machineCode, names);
}

void ExpressionFunctionR1R3::compile(const Expr3 &expr, TrigonometricMode mode, bool machineCode, const String &names) {
  for(int i = 0; i < ARRAYSIZE(m_expr); i++) {
    m_expr[i].compile(expr.getExprText(i), mode, machineCode, names);
  }
}

Point3D ExpressionFunctionR1R3::operator()(const Real &t) {
  return Point3D(m_expr[0](t), m_expr[1](t), m_expr[2](t));
}

void ExpressionFunctionR1R3::setTime(const Real &time) {
  for(int i = 0; i < ARRAYSIZE(m_expr); i++) {
    m_expr[i].setTime(time);
  }
}

const Real &ExpressionFunctionR1R3::getTime() const {
  return m_expr[0].getTime();
}

FunctionWithTimeTemplate<FunctionR1R3> *ExpressionFunctionR1R3::clone() const {
  return new ExpressionFunctionR1R3(*this);
}

}; // namespace Expr
