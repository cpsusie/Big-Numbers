#pragma once

#include "GraphParameters.h"

class PointGraphParameters : public GraphParameters {
public:
  PointGraphParameters(const String &name, COLORREF color, UINT rollAvgSize, GraphStyle style)
    : GraphParameters(name, color, rollAvgSize, style)
  {
  }
  virtual GraphParameters *clone() const {
    return new PointGraphParameters(*this);
  }
  void putDataToDoc(XMLDoc &doc) {
    throwUnsupportedOperationException(__TFUNCTION__);
  }
  void getDataFromDoc(XMLDoc &doc) {
    throwUnsupportedOperationException(__TFUNCTION__);
  }
  int getType() const {
    return POINTGRAPH;
  }
};
