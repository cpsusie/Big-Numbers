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
  const ExpressionVariable *xvp = m_expr.getVariable(_T("x"));
  const ExpressionVariable *yvp = m_expr.getVariable(_T("y"));
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

IsoCurveGraph::IsoCurveGraph(const IsoCurveGraphParameters &param) : Graph(new IsoCurveGraphParameters(param)) {
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
  const IsoCurveGraphParameters &param = (IsoCurveGraphParameters&)getParam();
  curveFinder.findCurve(param.m_cellSize, param.m_boundingBox);
  findDataRange();
}

void IsoCurveGraph::findDataRange() {
  if(isEmpty()) {
    m_range.init(-1,1,-1,1);
  } else {
    const LineSegment *lsp = &m_lineArray[0];
    const LineSegment *end = &m_lineArray.last();
    m_range =  m_pointArray[lsp->m_i1];
    m_range += m_pointArray[lsp->m_i2];
    while(lsp++ < end) {
      m_range += m_pointArray[lsp->m_i1];
      m_range += m_pointArray[lsp->m_i2];
    }
  }
}

void IsoCurveGraph::setTrigoMode(TrigonometricMode mode) {
  IsoCurveGraphParameters &param = (IsoCurveGraphParameters&)getParam();
  if(mode != param.m_trigonometricMode) {
    param.m_trigonometricMode = mode;
    calculate();
  }
}

void IsoCurveGraph::paint(CCoordinateSystem &cs) {
  if(isEmpty()) {
    return;
  }
  Viewport2D        &vp    = cs.getViewport();
  const LineSegment *lsp   = &m_lineArray[0];
  const LineSegment *end   = &m_lineArray.last();
  const COLORREF     color = getParam().m_color;
  switch(getParam().m_style) {
  case GSCURVE :
    { CPen pen;
      pen.CreatePen(PS_SOLID, 1, color);
      vp.SelectObject(&pen);
      for(;lsp <= end; lsp++) {
        const Point2D &p1 = m_pointArray[lsp->m_i1];
        const Point2D &p2 = m_pointArray[lsp->m_i2];
        if(pointDefined(p1) && pointDefined(p1)) {
          vp.MoveTo(p1);
          vp.LineTo(p2);
          cs.setOccupiedLine(p1,p2);
        }
      }
    }
    break;
  case GSPOINT :
    { Point2DArray tmp(2*m_pointArray.size());
      for(;lsp <= end; lsp++) {
        const Point2D &p1 = m_pointArray[lsp->m_i1];
        const Point2D &p2 = m_pointArray[lsp->m_i2];
        if(pointDefined(p1)) {
          vp.SetPixel(p1, color);
          tmp.add(p1);
        }
        if(pointDefined(p2)) {
          vp.SetPixel(p2, color);
          tmp.add(p2);
        }
      }
      cs.setOccupiedPoints(tmp);
    }
    break;
  case GSCROSS :
    { Point2DArray tmp(2*m_pointArray.size());
      for(;lsp <= end; lsp++) {
        const Point2D &p1 = m_pointArray[lsp->m_i1];
        const Point2D &p2 = m_pointArray[lsp->m_i2];
        if(pointDefined(p1)) {
          vp.paintCross(p1, color, 6);
          tmp.add(p1);
        }
        if(pointDefined(p2)) {
          vp.paintCross(p2, color, 6);
          tmp.add(p2);
        }
      }
      cs.setOccupiedPoints(tmp);
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
    return EMPTY_DISTANCE;
  }
  const size_t       n  = m_lineArray.size();
  const LineSegment *ls = &m_lineArray[0];

  switch(getParam().m_style) {
  case GSCURVE:
    { double minDist = EMPTY_DISTANCE;
      for(size_t i = 0; i < n; i++, ls++) {
        try {
          const Point2D &p1 = m_pointArray[ls->m_i1];
          const Point2D &p2 = m_pointArray[ls->m_i2];
          const double dist = distanceFromLineSegment(tr.forwardTransform((Point2DP)p1), tr.forwardTransform(p2), (Point2DP)p);
          if(dist < minDist) {
            minDist = dist;
          }
        } catch(...) {
          // ignore
        }
      }
      return minDist;
    }
  case GSPOINT:
  case GSCROSS:
    { double minDist = EMPTY_DISTANCE;
      for(size_t i = 0; i < n; i++, ls++) {
        try {
          const Point2D &p1 = m_pointArray[ls->m_i1];
          double dist = ::distance(tr.forwardTransform(p1), (Point2DP)p);
          if(minDist < 0 || dist < minDist) {
            minDist = dist;
          }
        } catch(...) {
          // ignore
        }
        try {
          const Point2D &p2 = m_pointArray[ls->m_i2];
          double dist = ::distance(tr.forwardTransform(p2), (Point2DP)p);
          if(minDist < 0 || dist < minDist) {
            minDist = dist;
          }
        } catch (...) {
          // ignore
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
  if(isEmpty()) return 0;
  const size_t       n      = m_lineArray.size();
  const LineSegment *ls     = &m_lineArray[0];
  double             result = max(0, m_pointArray[ls->m_i1].x);
  result = getMinPositive(m_pointArray[ls->m_i2].x, result);
  for(size_t i = 0; i < n; i++, ls++) {
    result = getMinPositive(m_pointArray[ls->m_i1].x, result);
    result = getMinPositive(m_pointArray[ls->m_i2].x, result);
  }
  return result;
}

double IsoCurveGraph::getSmallestPositiveY() const {
  if(isEmpty()) return 0;
  const size_t       n      = m_lineArray.size();
  const LineSegment *ls     = &m_lineArray[0];
  double             result = max(0, m_pointArray[ls->m_i1].y);
  result = getMinPositive(m_pointArray[ls->m_i2].y, result);
  for(size_t i = 0; i < n; i++, ls++) {
    result = getMinPositive(m_pointArray[ls->m_i1].y, result);
    result = getMinPositive(m_pointArray[ls->m_i2].y, result);
  }
  return result;
}

class IsoCurveY0Function : public Function {
private:
  IsoCurveGraphEvaluator m_eval;
public:
  IsoCurveY0Function(const IsoCurveGraph *graph) : m_eval((IsoCurveGraph*)graph) {
  }
  Real operator()(const Real &x) {
    return m_eval.evaluate(Point2D(x,0));
  }
};

CompactDoubleArray IsoCurveGraph::findZeroes(const DoubleInterval &i) const {
  return ::findZeroes(IsoCurveY0Function(this), i);
}
