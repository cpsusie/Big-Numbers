#include "pch.h"
#include <Math/Expression/ExpressionFunctionR2R1.h>

namespace Expr {

ExpressionFunctionR2R1::ExpressionFunctionR2R1(const String &expr, TrigonometricMode mode, bool machineCode, const String &names) {
  compile(expr, mode, machineCode, names);
}

ExpressionFunctionR2R1::ExpressionFunctionR2R1(const ExpressionFunctionR2R1 &src)
: m_expr( src.m_expr )
, m_names(src.m_names)
{
  setVariables();
}

ExpressionFunctionR2R1 &ExpressionFunctionR2R1::operator=(const ExpressionFunctionR2R1 &src) {
  cleanup();
  m_expr  = src.m_expr;
  m_names = src.m_names;
  setVariables();
  return *this;
}

void ExpressionFunctionR2R1::compile(const String &expr, TrigonometricMode mode, bool machineCode, const String &names) {
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

Real *ExpressionFunctionR2R1::getVariableByName(const String &name) {
  const Expr::ExpressionVariable *var = m_expr.getVariable(name);
  return (var == NULL) ? &m_dummy : &m_expr.getValueRef(*var);
}

void ExpressionFunctionR2R1::setVariables() {
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

void ExpressionFunctionR2R1::cleanup() {
  m_expr.clear();
  m_names = EMPTYSTRING;
  initVariables();
}

Real ExpressionFunctionR2R1::operator()(const Real &x, const Real &y) {
  *m_var[0] = x;
  *m_var[1] = y;
  return m_expr.evaluate();
}

Real ExpressionFunctionR2R1::operator()(const Point2D &p) {
  *m_var[0] = p.x;
  *m_var[1] = p.y;
  return m_expr.evaluate();
}

void ExpressionFunctionR2R1::setTime(const Real &time) {
  *m_var[2] = time;
}

const Real &ExpressionFunctionR2R1::getTime() const {
  return *m_var[2];
}

FunctionWithTimeTemplate<FunctionR2R1> *ExpressionFunctionR2R1::clone() const {
  return new ExpressionFunctionR2R1(*this);
}

}; // namespace Expr
