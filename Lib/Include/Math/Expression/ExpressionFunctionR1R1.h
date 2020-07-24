#pragma once

#include <Math/MathLib.h>
#include "Expression.h"

namespace Expr {

class ExpressionFunctionR1R1 : public FunctionWithTimeTemplate<FunctionR1R1> {
private:
  Expression m_expr;
  String     m_names;
  Real       m_dummy,*m_var[2];

  inline void initVariables() {
    m_var[0] = m_var[1] = &m_dummy;
  }

  void setVariables();
  void cleanup();
  Real *getVariableByName(const String &name);
public:
  ExpressionFunctionR1R1() {
    initVariables();
  }
  // throws Exception on compile-error
  ExpressionFunctionR1R1(const String &expr, TrigonometricMode mode = RADIANS, bool machineCode = true, const String &names=_T("x,time"));
  ExpressionFunctionR1R1(           const ExpressionFunctionR1R1 &src);
  ExpressionFunctionR1R1 &operator=(const ExpressionFunctionR1R1 &src);

  // throws Exception on compile-error
  void compile(const String &expr, TrigonometricMode mode = RADIANS, bool machineCode=true, const String &names=_T("x,time"));
  Real operator()(const Real &x);
  void        setTime(const Real &time);
  const Real &getTime() const;
  FunctionWithTimeTemplate<FunctionR1R1> *clone() const;
};

}; // namespace Expr
