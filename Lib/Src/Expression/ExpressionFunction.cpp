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
  initx();
}

ExpressionFunction &ExpressionFunction::operator=(ExpressionFunction &src) {
  cleanup();
  m_expr    = src.m_expr;
  m_varName = src.getVarName();
  initx();
  return *this;
}

void ExpressionFunction::compile(const String &expr, const String &name, TrigonometricMode mode, bool machineCode) {
  cleanup();
  StringArray errors;
  m_expr.setTrigonometricMode(mode);
  if(!m_expr.compile(expr, errors, machineCode)) {
    throwException(errors.first());
  }
  if(m_expr.getReturnType() != EXPR_RETURN_REAL) {
    throwException(_T("Returntype of expression not real"));
  }
  m_varName = name;
  initx();
}

void ExpressionFunction::initx() {
  if(m_expr.hasSyntaxTree()) {
    const ExpressionVariable *xvp = m_expr.getVariable(m_varName);
    m_x = xvp ? &m_expr.getValueRef(*xvp) : &m_dummyX;
  } else {
    m_x = &m_dummyX;
  }
}

void ExpressionFunction::cleanup() {
  m_varName = EMPTYSTRING;
  m_x       = &m_dummyX;
  m_expr.clear();
}

Real ExpressionFunction::operator()(const Real &x) {
  *m_x = x;
  return m_expr.evaluate();
}

}; // namespace Expr
