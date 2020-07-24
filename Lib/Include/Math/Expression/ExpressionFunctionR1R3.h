#pragma once

#include <Math/Point3D.h>
#include "Expr3.h"
#include "ExpressionFunctionR1R1.h"

namespace Expr {

class ExpressionFunctionR1R3 : public FunctionWithTimeTemplate<FunctionR1R3> {
private:
  ExpressionFunctionR1R1 m_expr[3];
public:
  ExpressionFunctionR1R3() {
  }
  // throws Exception on compile-error
  ExpressionFunctionR1R3(const Expr3 &expr, TrigonometricMode mode = RADIANS, bool machineCode = true, const String &names=_T("t,time"));

  // throws Exception on compile-error
  void compile(const Expr3 &expr, TrigonometricMode mode = RADIANS, bool machineCode = true, const String &names=_T("t,time"));
  Point3D operator()(const Real &t);
  void        setTime(const Real &time);
  const Real &getTime() const;
  FunctionWithTimeTemplate<FunctionR1R3> *clone() const;
};

}; // namespace Expr
