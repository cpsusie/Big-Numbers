#pragma once

#include "D3FunctionParamPersistentData.h"
#include "IsoSurfaceParameters.h"

class ExprIsoSurfaceParameters : public IsoSurfaceParameters, public FunctionImageParamPersistentData {
private:
  void getDataFromDocOld(XMLDoc &doc);
public:
  String m_expr;
  bool   m_machineCode;

  ExprIsoSurfaceParameters() : m_machineCode(true) {
  }

  void putDataToDoc(XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);

  FunctionDomainRangeType getType() const {
    return TYPE_ISOSURFACE;
  }
};
