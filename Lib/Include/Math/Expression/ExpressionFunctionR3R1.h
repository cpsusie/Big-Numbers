#pragma once

#include <Math/Point3D.h>
#include "Expression.h"

namespace Expr {

class ExpressionFunctionR3R1 : public FunctionWithTimeTemplate<FunctionR3R1> {
private:
  Expression m_expr;
  Real       m_dummy,*m_x,*m_y,*m_z,*m_t;

  inline void initVariables() {
    m_x = m_y = m_z = m_t = &m_dummy;
  }
  void setVariables();
  void cleanup();
  Real *getVariableByName(const String &name);
public:
  ExpressionFunctionR3R1() {
    initVariables();
  }
  // throws Exception on compile-error
  ExpressionFunctionR3R1(const String &expr, TrigonometricMode mode = RADIANS, bool machineCode = true);
  ExpressionFunctionR3R1(           const ExpressionFunctionR3R1 &src);
  ExpressionFunctionR3R1 &operator=(const ExpressionFunctionR3R1 &src);

  // throws Exception on compile-error
  void compile(const String &expr, TrigonometricMode mode = RADIANS, bool machineCode = true);
  Real operator()(const Real &x, const Real &y, const Real &z);
  Real operator()(const Point3D &p);
  void        setTime(const Real &time);
  const Real &getTime() const;
  FunctionWithTimeTemplate<FunctionR3R1> *clone() const;
};

}; // namespace Expr
