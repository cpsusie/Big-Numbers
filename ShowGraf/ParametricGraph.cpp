#include "stdafx.h"
#include <Math/Expression/ExpressionFunction.h>
#include "ParametricGraph.h"

ParametricGraph::ParametricGraph(const ParametricGraphParameters &param) : PointGraph(new ParametricGraphParameters(param)) {
  calculate();
}

void ParametricGraph::calculate() {
  clear();
  const ParametricGraphParameters &param = (ParametricGraphParameters&)getParam();
  ExpressionFunction Xt(param.getExprXText(true), _T("t"), param.getTrigonometricMode());
  ExpressionFunction Yt(param.getExprYText(true), _T("t"), param.getTrigonometricMode());
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

void ParametricGraph::paint(CCoordinateSystem &cs) {
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
