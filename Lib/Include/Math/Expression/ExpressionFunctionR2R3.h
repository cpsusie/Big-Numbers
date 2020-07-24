#pragma once

#include <Math/Point2D3D.h>
#include "Expr3.h"
#include "ExpressionFunctionR2R1.h"

namespace Expr {

class ExpressionFunctionR2R3 : public FunctionWithTimeTemplate<FunctionR2R3> {
private:
  ExpressionFunctionR2R1 m_expr[3];
public:
  ExpressionFunctionR2R3() {
  }
  // throws Exception on compile-error
  ExpressionFunctionR2R3(const Expr3 &expr, TrigonometricMode mode = RADIANS, bool machineCode = true, const String &names=_T("t,s,time"));

  // throws Exception on compile-error
  void compile(const Expr3 &expr, TrigonometricMode mode = RADIANS, bool machineCode = true, const String &names=_T("t,s,time"));
  Point3D operator()(const Real &t, const Real &s);
  Point3D operator()(const Point2D &p);
  void        setTime(const Real &time);
  const Real &getTime() const;
  FunctionWithTimeTemplate<FunctionR2R3> *clone() const;
};

}; // namespace Expr
