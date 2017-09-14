#include "stdafx.h"
#include <Math/Expression/ExpressionFunction.h>
#include "FunctionGraph.h"

class FunctionGraphFunction : public ExpressionFunction {
public:
  inline FunctionGraphFunction(const FunctionGraph *graph) {
    const FunctionGraphParameters &param = (FunctionGraphParameters&)graph->getParam();
    compile(param.m_expr, _T("x"), param.m_trigonometricMode);
  }
};

FunctionGraph::FunctionGraph(const FunctionGraphParameters &param) : PointGraph(new FunctionGraphParameters(param)) {
  calculate();
}

void FunctionGraph::calculate() {
  clear();
  FunctionGraphFunction          f(this);
  const FunctionGraphParameters &param     = (FunctionGraphParameters&)getParam();
  const int                      stepCount = param.m_steps;
  const double                   step      = param.m_interval.getLength() / stepCount;
  Real                           x         = param.m_interval.getMin();
  for(int i = 0; i <= stepCount; x += step, i++) {
    try {
      if(i == stepCount) {
        x = param.m_interval.getMax();
      }
      addPoint(Point2D(x,f(x)));
    } catch(Exception e) {
      // ignore
    }
  }
  updateDataRange();
}

void FunctionGraph::setTrigoMode(TrigonometricMode mode) {
  FunctionGraphParameters &param = (FunctionGraphParameters&)getParam();
  if(mode != param.m_trigonometricMode) {
    param.m_trigonometricMode = mode;
    calculate();
  }
}

void FunctionGraph::paint(CCoordinateSystem &cs) {
  __super::paint(cs);
/*
  if(m_image.isEmpty()) return;
  Viewport2D &vp = cs.getViewport();
  CPoint topLeft(30,30);
  PixRect::bitBlt(*vp.getDC(), topLeft, m_image.size(), SRCCOPY, m_image.getImage(), ORIGIN);
  const CRect imageRect(topLeft, m_image.getImage()->getSize());
  cs.getOccupationMap().setOccupiedRect(imageRect);
*/
}

GraphZeroesResultArray FunctionGraph::findZeroes(const DoubleInterval &i) const {
  return makeZeroesResult(::findZeroes(FunctionGraphFunction(this),i));
}
