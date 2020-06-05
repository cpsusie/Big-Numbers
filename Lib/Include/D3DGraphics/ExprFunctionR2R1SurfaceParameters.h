#pragma once

#include "D3FunctionParamPersistentData.h"
#include "FunctionR2R1SurfaceParameters.h"

class ExprFunctionR2R1SurfaceParameters : public FunctionR2R1SurfaceParameters, public FunctionImageParamPersistentData {
private:
  void getDataFromDocOld1(XMLDoc &doc);
  void getDataFromDocOld2(XMLDoc &doc);
public:
  String         m_expr;
  bool           m_machineCode;

  ExprFunctionR2R1SurfaceParameters() : m_machineCode(true) {
  }
  void putDataToDoc(  XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);
  FunctionDomainRangeType getType() const {
    return TYPE_FUNCTIONR2R1SURFACE;
  }
};

