#pragma once

#include <MFCUtil/FunctionImageParamPersistentData.h>
#include "FunctionR2R1SurfaceParameters.h"

class ExprFunctionR2R1SurfaceParameters : public FunctionR2R1SurfaceParameters, public FunctionImageParamPersistentData {
public:
  String m_expr;
  bool   m_machineCode;

  ExprFunctionR2R1SurfaceParameters() : m_machineCode(true) {
  }
  void putDataToDoc(  XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);

  FunctionDomainRangeDimension getType() const {
    return TYPE_FUNCTIONR2R1SURFACE;
  }
};

