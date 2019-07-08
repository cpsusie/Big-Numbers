#pragma once

#include <MFCUtil/Viewport2D.h>
#include <MFCUtil/ExpressionGraphics.h>
#include "Graph.h"
#include "ParametricGraphParameters.h"

class ParametricGraph : public PointGraph {
private:
//  ExpressionImage m_image;
public:
  ParametricGraph(CCoordinateSystem &system, const ParametricGraphParameters &param);
  void calculate();
  void paint(CDC &dc);
  void setTrigoMode(TrigonometricMode mode);
  inline GraphType getType() const {
    return PARAMETRICGRAPH;
  }
  GraphZeroesResultArray findZeroes(const DoubleInterval &i) {
    return GraphZeroesResultArray(*this);
  }
};
