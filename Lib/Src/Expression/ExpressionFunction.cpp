#include "pch.h"
#include <Math/Expression/ExpressionFunction.h>

namespace Expr {

ExpressionFunction::ExpressionFunction(const String &expr, TrigonometricMode mode, bool machineCode, const String &name) {
  compile(expr, mode, machineCode, name);
}

ExpressionFunction::ExpressionFunction(ExpressionFunction &src)
: m_expr(src.m_expr)
, m_name(src.m_name)
{
  setVariables();
}

ExpressionFunction &ExpressionFunction::operator=(ExpressionFunction &src) {
  cleanup();
  m_expr = src.m_expr;
  m_name = src.m_name;
  setVariables();
  return *this;
}

void ExpressionFunction::compile(const String &expr, TrigonometricMode mode, bool machineCode, const String &name) {
  cleanup();
  StringArray errors;
  m_expr.setTrigonometricMode(mode);
  if(!m_expr.compile(expr, errors, machineCode)) {
    throwException(errors.first());
  }
  if(m_expr.getReturnType() != EXPR_RETURN_FLOAT) {
    throwException(_T("Returntype of expression not real"));
  }
  m_name = name;
  setVariables();
}

Real *ExpressionFunction::getVariableByName(const String &name) {
  const Expr::ExpressionVariable *var = m_expr.getVariable(name);
  return (var == NULL) ? &m_dummy : &m_expr.getValueRef(*var);
}

void ExpressionFunction::setVariables() {
  if(m_expr.hasSyntaxTree()) {
    m_var = getVariableByName(m_name);
  } else {
    initVariables();
  }
}

void ExpressionFunction::cleanup() {
  m_expr.clear();
  m_name = EMPTYSTRING;
  initVariables();
}

Real ExpressionFunction::operator()(const Real &x) {
  *m_var = x;
  return m_expr.evaluate();
}

}; // namespace Expr
