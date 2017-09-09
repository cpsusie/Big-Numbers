#include "stdafx.h"
#include <Math/Expression/Expression.h>
#include "FunctionGraph.h"

class FunctionGraphFunction : public Function {
private:
  FunctionGraph       &m_graph;
  Expression           m_expr;
  Real                 m_dummyX;
  Real                *m_x;
public:
  FunctionGraphFunction(const FunctionGraph *graph);
  Real operator()(const Real &x);
};

FunctionGraphFunction::FunctionGraphFunction(const FunctionGraph *graph)
: m_graph(*(FunctionGraph*)graph)
, m_expr(((FunctionGraphParameters&)graph->getParam()).m_trigonometricMode) {

  const FunctionGraphParameters &param = (FunctionGraphParameters&)m_graph.getParam();
  m_expr.compile(param.m_expr, true);
  const ExpressionVariable *xvp = m_expr.getVariable(_T("x"));
  m_x = xvp ? &m_expr.getValueRef(*xvp) : &m_dummyX;
}

Real FunctionGraphFunction::operator()(const Real &x) {
  *m_x = x;
  return m_expr.evaluate();
}

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
