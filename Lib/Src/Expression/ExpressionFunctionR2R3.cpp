#include "pch.h"
#include <Math/Expression/ExpressionFunctionR2R3.h>

namespace Expr {

ExpressionFunctionR2R3::ExpressionFunctionR2R3(const Expr3 &expr, TrigonometricMode mode, bool machineCode) {
  compile(expr, mode, machineCode);
}

void ExpressionFunctionR2R3::compile(const Expr3 &expr, TrigonometricMode mode, bool machineCode) {
  for(int i = 0; i < 3; i++) {
    m_expr[i].compile(expr.getExprText(i), mode, machineCode);
  }
}

Point3D ExpressionFunctionR2R3::operator()(const Real &s, const Real &t) {
  return Point3D(m_expr[0](s, t), m_expr[1](s, t), m_expr[2](s, t));
}

Point3D ExpressionFunctionR2R3::operator()(const Point2D &p) {
  return Point3D(m_expr[0](p), m_expr[1](p), m_expr[2](p));
}

void ExpressionFunctionR2R3::setTime(const Real &time) {
  for(int i = 0; i < 3; i++) {
    m_expr[i].setTime(time);
  }
}

const Real &ExpressionFunctionR2R3::getTime() const {
  return m_expr[0].getTime();
}

FunctionWithTimeTemplate<FunctionR2R3> *ExpressionFunctionR2R3::clone() const {
  return new ExpressionFunctionR2R3(*this);
}

Expr3::Expr3(const String &expr1, const String &expr2, const String &expr3, const String &commonText)
: m_commonText(commonText)
{
  add(expr1);
  add(expr2);
  add(expr3);
}

}; // namespace Expr
