#pragma once

#include <MFCUtil/ExpressionGraphics.h>
#include "PointGraph.h"
#include "FunctionGraphParameters.h"

class FunctionGraph : public PointGraph {
private:
//  ExpressionImage m_image;
public:
  FunctionGraph(CCoordinateSystem &system, const FunctionGraphParameters &param);
  void calculate();
  void paint(CDC &dc);
  void setTrigoMode(TrigonometricMode mode);
  inline GraphType getType() const {
    return FUNCTIONGRAPH;
  }
  GraphZeroesResultArray  findZeroes( const DoubleInterval &interval);
  GraphExtremaResultArray findExtrema(const DoubleInterval &interval, ExtremaType extremaType);
};
