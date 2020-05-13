#include "pch.h"
#include <Math/Expression/ExpressionFunctionR3R1.h>

namespace Expr {

ExpressionFunctionR3R1::ExpressionFunctionR3R1(const String &expr, TrigonometricMode mode, bool machineCode) {
  compile(expr, mode, machineCode);
}

ExpressionFunctionR3R1::ExpressionFunctionR3R1(const ExpressionFunctionR3R1 &src)
: m_expr(src.m_expr)
{
  setVariables();
}

ExpressionFunctionR3R1 &ExpressionFunctionR3R1::operator=(const ExpressionFunctionR3R1 &src) {
  cleanup();
  m_expr = src.m_expr;
  setVariables();
  return *this;
}

void ExpressionFunctionR3R1::compile(const String &expr, TrigonometricMode mode, bool machineCode) {
  cleanup();
  StringArray errors;
  m_expr.setTrigonometricMode(mode);
  if(!m_expr.compile(expr, errors, machineCode)) {
    throwException(errors.first());
  }
  if(m_expr.getReturnType() != EXPR_RETURN_FLOAT) {
    throwException(_T("Returntype of expression not real"));
  }
  setVariables();
}

Real *ExpressionFunctionR3R1::getVariableByName(const String &name) {
  const Expr::ExpressionVariable *var = m_expr.getVariable(name);
  return (var == NULL) ? &m_dummy : &m_expr.getValueRef(*var);
}

void ExpressionFunctionR3R1::setVariables() {
  if(m_expr.hasSyntaxTree()) {
    m_x = getVariableByName(_T("x"));
    m_y = getVariableByName(_T("y"));
    m_z = getVariableByName(_T("z"));
    m_t = getVariableByName(_T("t"));
  } else {
    initVariables();
  }
}

void ExpressionFunctionR3R1::cleanup() {
  m_expr.clear();
  initVariables();
}

Real ExpressionFunctionR3R1::operator()(const Real &x, const Real &y, const Real &z) {
  *m_x = x;
  *m_y = y;
  *m_z = z;
  return m_expr.evaluate();
}

Real ExpressionFunctionR3R1::operator()(const Point3D &p) {
  *m_x = p.x;
  *m_y = p.y;
  *m_z = p.z;
  return m_expr.evaluate();
}

void ExpressionFunctionR3R1::setTime(const Real &time) {
  *m_t = time;
}

const Real &ExpressionFunctionR3R1::getTime() const {
  return *m_t;
}

FunctionWithTimeTemplate<FunctionR3R1> *ExpressionFunctionR3R1::clone() const {
  return new ExpressionFunctionR3R1(*this);
}

}; // namespace Expr
