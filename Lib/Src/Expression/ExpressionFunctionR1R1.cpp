#include "pch.h"
#include <Math/Expression/ExpressionFunctionR1R1.h>

namespace Expr {

ExpressionFunctionR1R1::ExpressionFunctionR1R1(const String &expr, TrigonometricMode mode, bool machineCode, const String &name) {
  compile(expr, mode, machineCode, name);
}

ExpressionFunctionR1R1::ExpressionFunctionR1R1(const ExpressionFunctionR1R1 &src)
: m_expr( src.m_expr )
, m_names(src.m_names)
{
  setVariables();
}

ExpressionFunctionR1R1 &ExpressionFunctionR1R1::operator=(const ExpressionFunctionR1R1 &src) {
  cleanup();
  m_expr  = src.m_expr;
  m_names = src.m_names;
  setVariables();
  return *this;
}

void ExpressionFunctionR1R1::compile(const String &expr, TrigonometricMode mode, bool machineCode, const String &names) {
  cleanup();
  StringArray errors;
  m_expr.setTrigonometricMode(mode);
  if(!m_expr.compile(expr, errors, machineCode)) {
    throwException(errors.first());
  }
  if(m_expr.getReturnType() != EXPR_RETURN_FLOAT) {
    throwException(_T("Returntype of expression not real"));
  }
  m_names = names;
  setVariables();
}

Real *ExpressionFunctionR1R1::getVariableByName(const String &name) {
  const Expr::ExpressionVariable *var = m_expr.getVariable(name);
  return (var == nullptr) ? &m_dummy : &m_expr.getValueRef(*var);
}

void ExpressionFunctionR1R1::setVariables() {
  if(m_expr.hasSyntaxTree()) {
    UINT i = 0;
    for(Tokenizer tok(m_names.cstr(), _T(",")); tok.hasNext() && i < ARRAYSIZE(m_var); i++) {
      const String varName = tok.next();
      m_var[i] = getVariableByName(varName);
    }
  } else {
    initVariables();
  }
}

void ExpressionFunctionR1R1::cleanup() {
  m_expr.clear();
  m_names = EMPTYSTRING;
  initVariables();
}

Real ExpressionFunctionR1R1::operator()(const Real &x) {
  *m_var[0] = x;
  return m_expr.evaluate();
}

void ExpressionFunctionR1R1::setTime(const Real &time) {
  *m_var[1] = time;
}
const Real &ExpressionFunctionR1R1::getTime() const {
  return *m_var[1];
}

FunctionWithTimeTemplate<FunctionR1R1> *ExpressionFunctionR1R1::clone() const {
  return new ExpressionFunctionR1R1(*this);
}

}; // namespace Expr
