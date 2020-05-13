#include "pch.h"
#include <Math/Expression/ExpressionFunctionR2R1.h>

namespace Expr {

ExpressionFunctionR2R1::ExpressionFunctionR2R1(const String &expr, TrigonometricMode mode, bool machineCode) {
  compile(expr, mode, machineCode);
}

ExpressionFunctionR2R1::ExpressionFunctionR2R1(const ExpressionFunctionR2R1 &src)
: m_expr(src.m_expr)
{
  setVariables();
}

ExpressionFunctionR2R1 &ExpressionFunctionR2R1::operator=(const ExpressionFunctionR2R1 &src) {
  cleanup();
  m_expr = src.m_expr;
  setVariables();
  return *this;
}

void ExpressionFunctionR2R1::compile(const String &expr, TrigonometricMode mode, bool machineCode) {
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

Real *ExpressionFunctionR2R1::getVariableByName(const String &name) {
  const Expr::ExpressionVariable *var = m_expr.getVariable(name);
  return (var == NULL) ? &m_dummy : &m_expr.getValueRef(*var);
}

void ExpressionFunctionR2R1::setVariables() {
  if(m_expr.hasSyntaxTree()) {
    m_x = getVariableByName(_T("x"));
    m_y = getVariableByName(_T("y"));
    m_t = getVariableByName(_T("t"));
  } else {
    initVariables();
  }
}

void ExpressionFunctionR2R1::cleanup() {
  m_expr.clear();
  initVariables();
}

Real ExpressionFunctionR2R1::operator()(const Real &x, const Real &y) {
  *m_x = x;
  *m_y = y;
  return m_expr.evaluate();
}

Real ExpressionFunctionR2R1::operator()(const Point2D &p) {
  *m_x = p.x;
  *m_y = p.y;
  return m_expr.evaluate();
}

void ExpressionFunctionR2R1::setTime(const Real &time) {
  *m_t = time;
}

const Real &ExpressionFunctionR2R1::getTime() const {
  return *m_t;
}

FunctionWithTimeTemplate<FunctionR2R1> *ExpressionFunctionR2R1::clone() const {
  return new ExpressionFunctionR2R1(*this);
}

}; // namespace Expr
