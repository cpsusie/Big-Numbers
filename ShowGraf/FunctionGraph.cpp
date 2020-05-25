#include "stdafx.h"
#include <Math/Expression/ExpressionFunction.h>
#include "FunctionGraph.h"

class FunctionGraphFunction : public ExpressionFunction {
public:
  inline FunctionGraphFunction(const FunctionGraph *graph) {
    const FunctionGraphParameters &param = (FunctionGraphParameters&)graph->getParam();
    compile(param.getExprText(), param.getTrigonometricMode(),true, _T("x"));
  }
};

FunctionGraph::FunctionGraph(CCoordinateSystem &system, const FunctionGraphParameters &param)
: PointGraph(system, param.clone())
{
  calculate();
}

void FunctionGraph::calculate() {
  clear();
  FunctionGraphFunction          f(this);
  const FunctionGraphParameters &param     = (FunctionGraphParameters&)getParam();
  const UINT                     stepCount = param.getSteps();
  const double                   step      = fabs(param.getInterval().getLength() / stepCount);
  Real                           x         = param.getInterval().getMin();
  for(UINT i = 0; i <= stepCount; x += step, i++) {
    try {
      if(i == stepCount) {
        x = param.getInterval().getMax();
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
  if(param.setTrigonometricMode(mode) != mode) {
    calculate();
  }
}

void FunctionGraph::paint(CDC &dc) {
  __super::paint(dc);
/*
  if(m_image.isEmpty()) return;
  Viewport2D &vp = cs.getViewport();
  CPoint topLeft(30,30);
  PixRect::bitBlt(*vp.getDC(), topLeft, m_image.size(), SRCCOPY, m_image.getImage(), ORIGIN);
  const CRect imageRect(topLeft, m_image.getImage()->getSize());
  cs.getOccupationMap().setOccupiedRect(imageRect);
*/
}

GraphZeroesResultArray FunctionGraph::findZeroes(const DoubleInterval &interval) {
  return makeZeroesResult(::findZeroes(FunctionGraphFunction(this),interval));
}

GraphExtremaResultArray FunctionGraph::findExtrema(const DoubleInterval &interval, ExtremaType extremaType) {
  Point2DArray pa;
  pa.add(findExtremum(FunctionGraphFunction(this), interval, extremaType==EXTREMA_TYPE_MAX));
  return makeExtremaResult(extremaType, pa);
}
