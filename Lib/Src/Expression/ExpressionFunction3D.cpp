#include "pch.h"
#include <Math/Expression/ExpressionFunction3D.h>

namespace Expr {

ExpressionFunction3D::ExpressionFunction3D(const String &expr, TrigonometricMode mode, bool machineCode) {
  compile(expr, mode, machineCode);
}

ExpressionFunction3D::ExpressionFunction3D(const ExpressionFunction3D &src)
: m_expr(src.m_expr)
{
  setVariables();
}

ExpressionFunction3D &ExpressionFunction3D::operator=(const ExpressionFunction3D &src) {
  cleanup();
  m_expr = src.m_expr;
  setVariables();
  return *this;
}

void ExpressionFunction3D::compile(const String &expr, TrigonometricMode mode, bool machineCode) {
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

Real *ExpressionFunction3D::getVariableByName(const String &name) {
  const Expr::ExpressionVariable *var = m_expr.getVariable(name);
  return (var == NULL) ? &m_dummy : &m_expr.getValueRef(*var);
}

void ExpressionFunction3D::setVariables() {
  if(m_expr.hasSyntaxTree()) {
    m_x = getVariableByName(_T("x"));
    m_y = getVariableByName(_T("y"));
    m_z = getVariableByName(_T("z"));
    m_t = getVariableByName(_T("t"));
  } else {
    initVariables();
  }
}

void ExpressionFunction3D::cleanup() {
  m_expr.clear();
  initVariables();
}

Real ExpressionFunction3D::operator()(const Real &x, const Real &y, const Real &z) {
  *m_x = x;
  *m_y = y;
  *m_z = z;
  return m_expr.evaluate();
}

Real ExpressionFunction3D::operator()(const Point3D &p) {
  *m_x = p.x;
  *m_y = p.y;
  *m_z = p.z;
  return m_expr.evaluate();
}

void ExpressionFunction3D::setTime(const Real &time) {
  *m_t = time;
}

const Real &ExpressionFunction3D::getTime() const {
  return *m_t;
}

FunctionWithTimeTemplate<Function3D> *ExpressionFunction3D::clone() const {
  return new ExpressionFunction3D(*this);
}

}; // namespace Expr
