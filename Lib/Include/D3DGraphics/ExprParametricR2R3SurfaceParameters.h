#pragma once

#include <Math/Expression/ExpressionFunctionR1R3.h>
#include <MFCUtil/FunctionImageParamPersistentData.h>
#include "ParametricR2R3SurfaceParameters.h"

using namespace Expr;

class ExprParametricR2R3SurfaceParameters : public ParametricR2R3SurfaceParameters, public FunctionImageParamPersistentData {
private:
  void getDataFromDocOld(XMLDoc &doc);
public:
  Expr3  m_expr;
  bool   m_machineCode;

  ExprParametricR2R3SurfaceParameters() : m_machineCode(true) {
  }
  void putDataToDoc(  XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);

  FunctionDomainRangeDimension getType() const {
    return TYPE_PARAMETRICR2R3SURFACE;
  }
};

