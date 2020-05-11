#pragma once

#include <Math/Point3D.h>
#include "Expression.h"

namespace Expr {

class ExpressionFunction3D : public FunctionWithTimeTemplate<Function3D> {
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
  ExpressionFunction3D() {
    initVariables();
  }
  // throws Exception on compile-error
  ExpressionFunction3D(const String &expr, TrigonometricMode mode = RADIANS, bool machineCode = true);
  ExpressionFunction3D(           const ExpressionFunction3D &src);
  ExpressionFunction3D &operator=(const ExpressionFunction3D &src);

  // throws Exception on compile-error
  void compile(const String &expr, TrigonometricMode mode = RADIANS, bool machineCode = true);
  Real operator()(const Real &x, const Real &y, const Real &z);
  Real operator()(const Point3D &p);
  void        setTime(const Real &time);
  const Real &getTime() const;
  FunctionWithTimeTemplate<Function3D> *clone() const;
};

}; // namespace Expr
