#pragma once

#include <Math/Point2D.h>
#include "Expression.h"

namespace Expr {

class ExpressionFunctionR2R1 : public FunctionWithTimeTemplate<FunctionR2R1> {
private:
  Expression m_expr;
  Real       m_dummy,*m_x,*m_y,*m_t;

  inline void initVariables() {
    m_x = m_y = m_t = &m_dummy;
  }
  void setVariables();
  void cleanup();
  Real *getVariableByName(const String &name);
public:
  ExpressionFunctionR2R1() {
    initVariables();
  }
  // throws Exception on compile-error
  ExpressionFunctionR2R1(const String &expr, TrigonometricMode mode = RADIANS, bool machineCode = true);
  ExpressionFunctionR2R1(           const ExpressionFunctionR2R1 &src);
  ExpressionFunctionR2R1 &operator=(const ExpressionFunctionR2R1 &src);

  // throws Exception on compile-error
  void compile(const String &expr, TrigonometricMode mode = RADIANS, bool machineCode = true);
  Real operator()(const Real &x, const Real &y);
  Real operator()(const Point2D &p);
  void        setTime(const Real &time);
  const Real &getTime() const;
  FunctionWithTimeTemplate<FunctionR2R1> *clone() const;
};

}; // namespace Expr
