#pragma once

#include "D3FunctionParamPersistentData.h"
#include "ParametricR1R3SurfaceParameters.h"
#include <Math/Expression/ExpressionFunctionR1R3.h>

using namespace Expr;

class ExprParametricR1R3SurfaceParameters : public ParametricR1R3SurfaceParameters, public FunctionImageParamPersistentData {
public:
  Expr3  m_expr;
  bool   m_machineCode;

  ExprParametricR1R3SurfaceParameters() : m_machineCode(true) {
  }
  void putDataToDoc(  XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);

  FunctionDomainRangeType getType() const {
    return TYPE_PARAMETRICR1R3SURFACE;
  }
};

