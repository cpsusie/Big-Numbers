#pragma once

#include <MFCUtil/FunctionImageParamPersistentData.h>
#include "IsoSurfaceParameters.h"

class ExprIsoSurfaceParameters : public IsoSurfaceParameters, public FunctionImageParamPersistentData {
public:
  String m_expr;
  bool   m_machineCode;

  ExprIsoSurfaceParameters() : m_machineCode(true) {
  }
  void putDataToDoc(  XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);

  FunctionDomainRangeDimension getType() const {
    return TYPE_ISOSURFACE;
  }
};
