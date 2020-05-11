#include "pch.h"
#include <Math/Expression/ExpressionFunction2D.h>

namespace Expr {

ExpressionFunction2D::ExpressionFunction2D(const String &expr, TrigonometricMode mode, bool machineCode) {
  compile(expr, mode, machineCode);
}

ExpressionFunction2D::ExpressionFunction2D(const ExpressionFunction2D &src)
: m_expr(src.m_expr)
{
  setVariables();
}

ExpressionFunction2D &ExpressionFunction2D::operator=(const ExpressionFunction2D &src) {
  cleanup();
  m_expr = src.m_expr;
  setVariables();
  return *this;
}

void ExpressionFunction2D::compile(const String &expr, TrigonometricMode mode, bool machineCode) {
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

Real *ExpressionFunction2D::getVariableByName(const String &name) {
  const Expr::ExpressionVariable *var = m_expr.getVariable(name);
  return (var == NULL) ? &m_dummy : &m_expr.getValueRef(*var);
}

void ExpressionFunction2D::setVariables() {
  if(m_expr.hasSyntaxTree()) {
    m_x = getVariableByName(_T("x"));
    m_y = getVariableByName(_T("y"));
    m_t = getVariableByName(_T("t"));
  } else {
    initVariables();
  }
}

void ExpressionFunction2D::cleanup() {
  m_expr.clear();
  initVariables();
}

Real ExpressionFunction2D::operator()(const Real &x, const Real &y) {
  *m_x = x;
  *m_y = y;
  return m_expr.evaluate();
}

Real ExpressionFunction2D::operator()(const Point2D &p) {
  *m_x = p.x;
  *m_y = p.y;
  return m_expr.evaluate();
}

void ExpressionFunction2D::setTime(const Real &time) {
  *m_t = time;
}

const Real &ExpressionFunction2D::getTime() const {
  return *m_t;
}

FunctionWithTimeTemplate<Function2D> *ExpressionFunction2D::clone() const {
  return new ExpressionFunction2D(*this);
}

}; // namespace Expr
