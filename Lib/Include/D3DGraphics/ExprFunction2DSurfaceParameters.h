#pragma once

#include <PersistentData.h>
#include "Function2DSurfaceParameters.h"

class ExprFunction2DSurfaceParameters : public Function2DSurfaceParameters, public PersistentData {
public:
  String         m_expr;
  bool           m_machineCode;

  ExprFunction2DSurfaceParameters() : m_machineCode(true) {
  }
  void putDataToDoc(XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);
  int getType() const {
    return PP_2DFUNCTION;
  }
};

