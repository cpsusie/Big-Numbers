#pragma once

#include <Math/MathLib.h>
#include "Expression.h"

namespace Expr {

class ExpressionFunction : public Function {
private:
  Expression           m_expr;
  String               m_varName;
  Real                 m_dummy, *m_x;

  inline void initVariables() {
    m_x = &m_dummy;
  }

  void setVariables();
  void cleanup();
  Real *getVariableByName(const String &name);
public:
  ExpressionFunction() {
    m_x = &m_dummy;
  }
  // throws Exception on compile-error
  ExpressionFunction(const String &expr, const String &name, TrigonometricMode mode = RADIANS, bool machineCode = true);
  ExpressionFunction(ExpressionFunction &src);
  ExpressionFunction &operator=(ExpressionFunction &src);

  // throws Exception on compile-error
  void compile(const String &expr, const String &name, TrigonometricMode mode = RADIANS, bool machineCode=true);
  Real operator()(const Real &x);
  const String getVarName() const {
    return m_varName;
  }
};

}; // namespace Expr
