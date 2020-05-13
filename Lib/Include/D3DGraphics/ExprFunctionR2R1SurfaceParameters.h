#pragma once

#include <PersistentData.h>
#include "FunctionR2R1SurfaceParameters.h"

class ExprFunctionR2R1SurfaceParameters : public FunctionR2R1SurfaceParameters, public PersistentData {
private:
  void getDataFromDocOld(XMLDoc &doc);
public:
  String         m_expr;
  bool           m_machineCode;

  ExprFunctionR2R1SurfaceParameters() : m_machineCode(true) {
  }
  void putDataToDoc(  XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);
  int getType() const {
    return PP_FUNCTIONR2R1SURFACE;
  }
};

