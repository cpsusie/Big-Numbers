#pragma once

#include <PersistentData.h>
#include "IsoSurfaceParameters.h"

class ExprIsoSurfaceParameters : public IsoSurfaceParameters, public PersistentData {
public:
  String m_expr;
  bool   m_machineCode;

  ExprIsoSurfaceParameters() : m_machineCode(true) {
  }

  void putDataToDoc(XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);

  int getType() const {
    return PP_ISOSURFACE;
  }
};
