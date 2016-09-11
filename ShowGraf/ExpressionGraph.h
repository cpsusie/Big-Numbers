#pragma once

#include <MFCUtil/Viewport2D.h>
#include <MFCUtil/ExpressionGraphics.h>
#include "Graph.h"
#include "ExpressionGraphParameters.h"

class ExpressionGraph : public PointGraph {
private:
  ExpressionImage m_image;
public:
  ExpressionGraph(ExpressionGraphParameters &param);
  void calculate();
  void paint(CCoordinateSystem &cs);
  void setTrigonometricMode(TrigonometricMode mode);
  inline GraphType getType() const {
    return EXPRESSIONGRAPH;
  }
};
