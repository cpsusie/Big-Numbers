#pragma once

#include <Math/Point2D3D.h>
#include "ExpressionFunctionR2R1.h"

namespace Expr {

class Expr3 : private StringArray {
private:
  String m_commonText;
public:
  inline Expr3() {
  }
  Expr3(const String &expr1, const String &expr2, const String &expr3, const String &commonText=EMPTYSTRING);

  inline String getExprText(UINT i) const {
    return m_commonText + _T(" ") + getRawText(i);
  }
  const String &getCommonText() const {
    return m_commonText;
  }
  inline String getRawText(UINT i) const {
    return (i<size()) ? (*this)[i] : EMPTYSTRING;
  }
};

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
