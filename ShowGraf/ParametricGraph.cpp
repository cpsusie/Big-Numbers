#include "stdafx.h"
#include <Math/Expression/ExpressionFunction.h>
#include "ParametricGraph.h"

ParametricGraph::ParametricGraph(const ParametricGraphParameters &param) : PointGraph(new ParametricGraphParameters(param)) {
  calculate();
}

void ParametricGraph::calculate() {
  clear();
  const ParametricGraphParameters &param = (ParametricGraphParameters&)getParam();
  ExpressionFunction Xt(param.m_commonText + param.m_exprX, _T("t"), param.m_trigonometricMode);
  ExpressionFunction Yt(param.m_commonText + param.m_exprY, _T("t"), param.m_trigonometricMode);
  const int          stepCount = param.m_steps;
  double             step      = (param.m_interval.getMax() - param.m_interval.getMin()) / stepCount;
  Real               t         = param.m_interval.getMin();
  for(int i = 0; i <= stepCount; t += step, i++) {
    try {
      if(i == stepCount) {
        t = param.m_interval.getMax();
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
  if(mode != param.m_trigonometricMode) {
    param.m_trigonometricMode = mode;
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
