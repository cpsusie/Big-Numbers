#include "stdafx.h"
#include <Math/Expression/Expression.h>
#include "ExpressionGraph.h"

ExpressionGraph::ExpressionGraph(ExpressionGraphParameters &param) : PointGraph(new ExpressionGraphParameters(param)) {
  calculate();
}

void ExpressionGraph::calculate() {
  clear();
  const ExpressionGraphParameters &param = *(ExpressionGraphParameters*)m_param;
  Expression expr(param.m_trigonometricMode);
  expr.compile(param.m_expr, true);
//  m_image = expressionToImage(theApp.m_device, expr, 18);
  Real                      dummyX;
  const ExpressionVariable *xvp       = expr.getVariable(_T("x"));
  Real                     &x         = xvp ? expr.getValueRef(*xvp) : dummyX;
  const int                 stepCount = param.m_steps;
  double                    step      = (param.m_interval.getMax() - param.m_interval.getMin()) / stepCount;
  x = param.m_interval.getMin();
  for(int i = 0; i <= stepCount; x += step, i++) {
    try {
      if(i == stepCount) {
        x = param.m_interval.getMax();
      }
      addPoint(Point2D(x,expr.evaluate()));
    } catch(Exception e) {
      // ignore
    }
  }
  updateDataRange();
}

void ExpressionGraph::setTrigonometricMode(TrigonometricMode mode) {
  ExpressionGraphParameters &param = *(ExpressionGraphParameters*)m_param;
  if(mode != param.m_trigonometricMode) {
    param.m_trigonometricMode = mode;
    calculate();
  }
}

void ExpressionGraph::paint(CCoordinateSystem &cs) {
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
