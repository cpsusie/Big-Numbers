#include "stdafx.h"
#include <Math/Expression/ExpressionFunctionR1R1.h>

#include "ParametricGraph.h"

ParametricGraph::ParametricGraph(CCoordinateSystem &system, const ParametricGraphParameters &param)
: PointGraph(system, param.clone())
{
  calculate();
}

void ParametricGraph::calculate() {
  clear();
  const ParametricGraphParameters &param = (ParametricGraphParameters&)getParam();
  ExpressionFunctionR1R1 Xt(param.getExprXText(true), param.getTrigonometricMode(), true, _T("t"));
  ExpressionFunctionR1R1 Yt(param.getExprYText(true), param.getTrigonometricMode(), true, _T("t"));
  const UINT         stepCount = param.getSteps();
  double             step      = fabs(param.getInterval().getLength() / stepCount);
  Real               t         = param.getInterval().getMin();
  for(UINT i = 0; i <= stepCount; t += step, i++) {
    try {
      if(i == stepCount) {
        t = param.getInterval().getMax();
      }
      addPoint(Point2D(Xt(t), Yt(t)));
    } catch(Exception e) {
      // ignore
    }
  }
  updateDataRange();
}

void ParametricGraph::setTrigoMode(TrigonometricMode mode) {
  ParametricGraphParameters &param = (ParametricGraphParameters&)getParam();
  if(param.setTrigonometricMode(mode) != mode) {
    calculate();
  }
}

void ParametricGraph::paint(CDC &dc) {
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
