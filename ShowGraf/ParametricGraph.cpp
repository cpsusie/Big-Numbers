#include "stdafx.h"
#include <Math/Expression/Expression.h>
#include "ParametricGraph.h"

ParametricGraph::ParametricGraph(ParametricGraphParameters &param) : PointGraph(new ParametricGraphParameters(param)) {
  calculate();
}

void ParametricGraph::calculate() {
  clear();
  const ParametricGraphParameters &param = *(ParametricGraphParameters*)m_param;
  Expression exprX(param.m_trigonometricMode);
  Expression exprY(param.m_trigonometricMode);
  exprX.compile(param.m_exprX, true);
  exprY.compile(param.m_exprY, true);
//  m_image = expressionToImage(theApp.m_device, expr, 18);
  Real                      dummyT;
  const ExpressionVariable *tvp       = exprX.getVariable(_T("t"));
  Real                     &tx        = tvp ? exprX.getValueRef(*tvp) : dummyT;
                            tvp       = exprY.getVariable(_T("t"));
  Real                     &ty        = tvp ? exprY.getValueRef(*tvp) : dummyT;
  const int                 stepCount = param.m_steps;
  double                    step      = (param.m_interval.getMax() - param.m_interval.getMin()) / stepCount;
  ty = tx = param.m_interval.getMin();
  for(int i = 0; i <= stepCount; tx += step, ty=tx, i++) {
    try {
      if(i == stepCount) {
        ty = tx = param.m_interval.getMax();
      }
      addPoint(Point2D(exprX.evaluate(), exprY.evaluate()));
    } catch(Exception e) {
      // ignore
    }
  }
  updateDataRange();
}

void ParametricGraph::setTrigonometricMode(TrigonometricMode mode) {
  ParametricGraphParameters &param = *(ParametricGraphParameters*)m_param;
  if(mode != param.m_trigonometricMode) {
    param.m_trigonometricMode = mode;
    calculate();
  }
}

void ParametricGraph::paint(CCoordinateSystem &cs) {
  PointGraph::paint(cs);
/*
  if(m_image.isEmpty()) return;
  Viewport2D &vp = cs.getViewport();
  CPoint topLeft(30,30);
  PixRect::bitBlt(*vp.getDC(), topLeft, m_image.size(), SRCCOPY, m_image.getImage(), ORIGIN);
  const CRect imageRect(topLeft, m_image.getImage()->getSize());
  cs.getOccupationMap().setOccupiedRect(imageRect);
*/
}
