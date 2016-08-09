#include "stdafx.h"
#include <Math/Expression/Expression.h>
#include "IsoCurveGraph.h"
#include "IsoCurveFinder.h"

class IsoCurveGraphEvaluator : public IsoCurveEvaluator {
private:
  IsoCurveGraph       &m_graph;
  Expression           m_expr;
  Real                 m_dummyX,m_dummyY;
  Real                *m_x,*m_y;
public:
  IsoCurveGraphEvaluator(IsoCurveGraph *graph);
  double evaluate(const Point2D &p);
  void receiveLineSegment(const LineSegment &line);
  const Point2DArray *m_pointArray;
};

IsoCurveGraphEvaluator::IsoCurveGraphEvaluator(IsoCurveGraph *graph) 
: m_graph(*graph)
, m_expr(((IsoCurveGraphParameters&)graph->getParam()).m_trigonometricMode) {

  const IsoCurveGraphParameters &param = (IsoCurveGraphParameters&)m_graph.getParam();
  m_expr.compile(param.m_expr, true);
  const ExpressionVariable *xvp = m_expr.getVariable("x");
  const ExpressionVariable *yvp = m_expr.getVariable("y");
  m_x = xvp ? &m_expr.getValueRef(*xvp) : &m_dummyX;
  m_y = yvp ? &m_expr.getValueRef(*yvp) : &m_dummyY;
}

double IsoCurveGraphEvaluator::evaluate(const Point2D &p) {
  *m_x = p.x;
  *m_y = p.y;
  return m_expr.evaluate();
}

void IsoCurveGraphEvaluator::receiveLineSegment(const LineSegment &line) {
  const size_t n = m_pointArray->size();
  if(n > m_graph.m_pointArray.size()) {
    for(size_t i = m_graph.m_pointArray.size(); i < n; i++) {
      m_graph.m_pointArray.add((*m_pointArray)[i]);
    }
  }
  m_graph.m_lineArray.add(line);
}

IsoCurveGraph::IsoCurveGraph(IsoCurveGraphParameters &param) : Graph(new IsoCurveGraphParameters(param)) {
  calculate();
}

void IsoCurveGraph::clear() {
  m_pointArray.clear();
  m_lineArray.clear();
  findDataRange();
}

void IsoCurveGraph::calculate() {
  clear();
  IsoCurveGraphEvaluator eval(this);
  IsoCurveFinder curveFinder(eval);
  eval.m_pointArray = &curveFinder.getPointArray();
  const IsoCurveGraphParameters &param = *(IsoCurveGraphParameters*)m_param;
  curveFinder.findCurve(param.m_cellSize, param.m_boundingBox);
  findDataRange();
}

void IsoCurveGraph::findDataRange() {
  if(isEmpty()) {
    m_range.init(-1,1,-1,1);
  } else {
    const int          n  = (int)m_lineArray.size();
    const LineSegment *ls = &m_lineArray[0];
    m_range.init(  m_pointArray[ls->m_i1]);
    m_range.update(m_pointArray[ls->m_i2]);
    for(int i = 0; i < n; i++, ls++) {
      m_range.update(m_pointArray[ls->m_i1]);
      m_range.update(m_pointArray[ls->m_i2]);
    }
  }
}

void IsoCurveGraph::setTrigonometricMode(TrigonometricMode mode) {
  IsoCurveGraphParameters &param = *(IsoCurveGraphParameters*)m_param;
  if(mode != param.m_trigonometricMode) {
    param.m_trigonometricMode = mode;
    calculate();
  }
}

void IsoCurveGraph::paint(Viewport2D &vp) {
  if(isEmpty()) {
    return;
  }
  const int          n     = (int)m_lineArray.size();
  const LineSegment *ls    = &m_lineArray[0];
  const COLORREF     color = getParam().m_color;

  switch(getParam().m_style) {
  case GSCURVE :
    { CPen pen;
      pen.CreatePen(PS_SOLID, 1, color);
      vp.SelectObject(&pen);
      for(int i = 0; i < n; i++, ls++) {
        const Point2D &p1 = m_pointArray[ls->m_i1];
        const Point2D &p2 = m_pointArray[ls->m_i2];
        if(pointDefined(p1) && pointDefined(p1)) {
          vp.MoveTo(p1);
          vp.LineTo(p2);
        }
      }
    }
    break;
  case GSPOINT :
    { for(int i = 0; i < n; i++, ls++) {
        const Point2D &p1 = m_pointArray[ls->m_i1];
        const Point2D &p2 = m_pointArray[ls->m_i2];
        if(pointDefined(p1)) {
          vp.SetPixel(p1, color);
        }
        if(pointDefined(p2)) {
          vp.SetPixel(p2, color);
        }
      }
    }
    break;
  case GSCROSS :
    { for(int i = 0; i < n; i++, ls++) {
        const Point2D &p1 = m_pointArray[ls->m_i1];
        const Point2D &p2 = m_pointArray[ls->m_i2];
        if(pointDefined(p1)) {
          vp.paintCross(p1, color, 6);
        }
        if(pointDefined(p2)) {
          vp.paintCross(p2, color, 6);
        }
      }
    }
    break;
  default:
    throwException(_T("Invalid style:%d"), getParam().m_style);
  }
}

bool IsoCurveGraph::isEmpty() const {
  return m_lineArray.isEmpty();
}

const DataRange &IsoCurveGraph::getDataRange() const {
  return m_range;
}

double IsoCurveGraph::distance(const CPoint &p, const RectangleTransformation &tr) const {
  if(isEmpty()) {
    return 1e40;
  }
  const int          n  = (int)m_lineArray.size();
  const LineSegment *ls = &m_lineArray[0];

  switch(getParam().m_style) {
  case GSCURVE:
    { double minDist = -1;
      for(int i = 0; i < n; i++, ls++) {
        const Point2D &p1 = m_pointArray[ls->m_i1];
        const Point2D &p2 = m_pointArray[ls->m_i2];
        const double dist = distanceFromLineSegment(tr.forwardTransform((Point2DP)p1), tr.forwardTransform(p2), (Point2DP)p);
        if(minDist < 0 || dist < minDist) {
          minDist = dist;
        }
      }
      return minDist;
    }
  case GSPOINT:
  case GSCROSS:
    { double minDist = -1;
      for(int i = 0; i < n; i++, ls++) {
        const Point2D &p1 = m_pointArray[ls->m_i1];
        const Point2D &p2 = m_pointArray[ls->m_i2];
        double dist = ::distance(tr.forwardTransform(p1), (Point2DP)p);
        if(minDist < 0 || dist < minDist) {
          minDist = dist;
        }
        dist = ::distance(tr.forwardTransform(p2), (Point2DP)p);
        if(minDist < 0 || dist < minDist) {
          minDist = dist;
        }
      }
      return minDist;
    }
  default:
    throwException(_T("Invalid style:%d"), getParam().m_style);
  }
  return 0;
}

double IsoCurveGraph::getSmallestPositiveX() const {
  if(isEmpty()) {
    return 0;
  } else {
    const int          n      = (int)m_lineArray.size();
    const LineSegment *ls     = &m_lineArray[0];
    double             result = max(0, m_pointArray[ls->m_i1].x);
    result = getMinPositive(m_pointArray[ls->m_i2].x, result);
    for(int i = 0; i < n; i++, ls++) {
      result = getMinPositive(m_pointArray[ls->m_i1].x, result);
      result = getMinPositive(m_pointArray[ls->m_i2].x, result);
    }
    return result;
  }
}

double IsoCurveGraph::getSmallestPositiveY() const {
  if(isEmpty()) {
    return 0;
  } else {
    const int          n      = (int)m_lineArray.size();
    const LineSegment *ls     = &m_lineArray[0];
    double             result = max(0, m_pointArray[ls->m_i1].y);
    result = getMinPositive(m_pointArray[ls->m_i2].y, result);
    for(int i = 0; i < n; i++, ls++) {
      result = getMinPositive(m_pointArray[ls->m_i1].y, result);
      result = getMinPositive(m_pointArray[ls->m_i2].y, result);
    }
    return result;
  }
}
