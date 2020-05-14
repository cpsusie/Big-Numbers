#pragma once

#include <Math/MathLib.h>
#include "Expression.h"

namespace Expr {

class ExpressionFunction : public Function {
private:
  Expression           m_expr;
  String               m_name;
  Real                 m_dummy, *m_var;

  inline void initVariables() {
    m_var = &m_dummy;
  }

  void setVariables();
  void cleanup();
  Real *getVariableByName(const String &name);
public:
  ExpressionFunction() {
    initVariables();
  }
  // throws Exception on compile-error
  ExpressionFunction(const String &expr, TrigonometricMode mode = RADIANS, bool machineCode = true, const String &name=_T("x"));
  ExpressionFunction(           ExpressionFunction &src);
  ExpressionFunction &operator=(ExpressionFunction &src);

  // throws Exception on compile-error
  void compile(const String &expr, TrigonometricMode mode = RADIANS, bool machineCode=true, const String &name=_T("x"));
  Real operator()(const Real &x);
  const String getVarName() const {
    return m_name;
  }
};

}; // namespace Expr
