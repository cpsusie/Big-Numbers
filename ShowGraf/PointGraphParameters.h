#pragma once

#include "GraphParameters.h"

class PointGraphParameters : public GraphParameters {
public:
  PointGraphParameters(const String &name, COLORREF color, const RollingAvg &rollingAvg, GraphStyle style)
    : GraphParameters(name, color, rollingAvg, style)
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
  GraphType getType() const {
    return POINTGRAPH;
  }
};
