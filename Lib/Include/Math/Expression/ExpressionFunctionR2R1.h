#pragma once

#include <Math/Point2D.h>
#include "Expression.h"

namespace Expr {

class ExpressionFunctionR2R1 : public FunctionWithTimeTemplate<FunctionR2R1> {
private:
  Expression m_expr;
  String     m_names;
  Real       m_dummy,*m_var[3];

  inline void initVariables() {
    m_var[0] = m_var[1] = m_var[2] = &m_dummy;
  }
  void setVariables();
  void cleanup();
  Real *getVariableByName(const String &name);
public:
  ExpressionFunctionR2R1() {
    initVariables();
  }
  // throws Exception on compile-error
  // names is comma-separated list of input-variable names, ex. "x,y,t", which will be used to set vaiables,
  // when operator()(x,y) us called
  ExpressionFunctionR2R1(const String &expr, TrigonometricMode mode = RADIANS, bool machineCode = true, const String &names=_T("x,y,t"));
  ExpressionFunctionR2R1(           const ExpressionFunctionR2R1 &src);
  ExpressionFunctionR2R1 &operator=(const ExpressionFunctionR2R1 &src);

  // throws Exception on compile-error
  void compile(const String &expr, TrigonometricMode mode = RADIANS, bool machineCode = true, const String &names=_T("x,y,t"));
  Real operator()(const Real &x, const Real &y);
  Real operator()(const Point2D &p);
  void        setTime(const Real &time);
  const Real &getTime() const;
  FunctionWithTimeTemplate<FunctionR2R1> *clone() const;
};

}; // namespace Expr
