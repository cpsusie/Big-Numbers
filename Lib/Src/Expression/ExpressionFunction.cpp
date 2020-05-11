#include "pch.h"
#include <Math/Expression/ExpressionFunction.h>

namespace Expr {

ExpressionFunction::ExpressionFunction(const String &expr, const String &name, TrigonometricMode mode, bool machineCode) {
  compile(expr, name, mode, machineCode);
}

ExpressionFunction::ExpressionFunction(ExpressionFunction &src)
: m_expr(   src.m_expr      )
, m_varName(src.getVarName())
{
  setVariables();
}

ExpressionFunction &ExpressionFunction::operator=(ExpressionFunction &src) {
  cleanup();
  m_expr    = src.m_expr;
  m_varName = src.getVarName();
  setVariables();
  return *this;
}

void ExpressionFunction::compile(const String &expr, const String &name, TrigonometricMode mode, bool machineCode) {
  cleanup();
  StringArray errors;
  m_expr.setTrigonometricMode(mode);
  if(!m_expr.compile(expr, errors, machineCode)) {
    throwException(errors.first());
  }
  if(m_expr.getReturnType() != EXPR_RETURN_FLOAT) {
    throwException(_T("Returntype of expression not real"));
  }
  m_varName = name;
  setVariables();
}

Real *ExpressionFunction::getVariableByName(const String &name) {
  const Expr::ExpressionVariable *var = m_expr.getVariable(name);
  return (var == NULL) ? &m_dummy : &m_expr.getValueRef(*var);
}

void ExpressionFunction::setVariables() {
  if(m_expr.hasSyntaxTree()) {
    m_x = getVariableByName(_T("x"));
  } else {
    initVariables();
  }
}

void ExpressionFunction::cleanup() {
  m_expr.clear();
  initVariables();
}

Real ExpressionFunction::operator()(const Real &x) {
  *m_x = x;
  return m_expr.evaluate();
}

}; // namespace Expr
