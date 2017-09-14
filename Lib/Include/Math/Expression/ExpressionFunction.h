#pragma once

#include <Math/MathLib.h>
#include "Expression.h"

class ExpressionFunction : public Function {
private:
  Expression           m_expr;
  String               m_varName;
  Real                 m_dummyX;
  Real                *m_x;

  void initx();
  void cleanup();
public:
  ExpressionFunction() {
    m_x = &m_dummyX;
  }
  // throws Exception on compile-error
  ExpressionFunction(const String &expr, const String &name, TrigonometricMode mode = RADIANS, bool machineCode = true);
  ExpressionFunction(ExpressionFunction &src);
  ExpressionFunction &operator=(ExpressionFunction &src);

  // throws Exception on compile-error
  void compile(const String &expr, const String &name, TrigonometricMode mode, bool machineCode=true);
  Real operator()(const Real &x);
  const String getVarName() const {
    return m_varName;
  }
};
