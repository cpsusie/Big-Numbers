#pragma once

#include <MFCUtil/Viewport2D.h>
#include "Graph.h"
#include "ExpressionGraphParameters.h"

class ExpressionGraph : public PointGraph {
public:
  ExpressionGraph(ExpressionGraphParameters &param);
  void calculate();
  void setTrigonometricMode(TrigonometricMode mode);
  GraphType getType() const {
    return EXPRESSIONGRAPH;
  }
};
