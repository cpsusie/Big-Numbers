#pragma once

#include <Math/Point2D.h>
#include "Expression.h"

namespace Expr {

class ExpressionFunction2D : public FunctionWithTimeTemplate<Function2D> {
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
  ExpressionFunction2D() {
    initVariables();
  }
  // throws Exception on compile-error
  ExpressionFunction2D(const String &expr, TrigonometricMode mode = RADIANS, bool machineCode = true);
  ExpressionFunction2D(           const ExpressionFunction2D &src);
  ExpressionFunction2D &operator=(const ExpressionFunction2D &src);

  // throws Exception on compile-error
  void compile(const String &expr, TrigonometricMode mode = RADIANS, bool machineCode = true);
  Real operator()(const Real &x, const Real &y);
  Real operator()(const Point2D &p);
  void        setTime(const Real &time);
  const Real &getTime() const;
  FunctionWithTimeTemplate<Function2D> *clone() const;
};

}; // namespace Expr
