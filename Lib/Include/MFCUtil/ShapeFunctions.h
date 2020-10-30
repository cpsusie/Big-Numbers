#pragma once

#include <Math/Rectangle2D.h>

class PointOperator {
public:
  virtual void apply(const CPoint &p) = 0;
};

class CurveOperator : public Point2DOperator {
private:
  Point2D m_currentPoint;
  bool    m_firstTime;

public:
  CurveOperator() {
    beginCurve();
  }
  void apply(const Point2D &p) override;
  virtual void line(const Point2D &from, const Point2D &to) = 0;

  virtual void beginCurve() {
    m_firstTime = true;
  }
  virtual void endCurve() {
  }

  const Point2D &getCurrentPoint() const {
    return m_currentPoint;
  }
  bool firstPointInCurve() const {
    return m_firstTime;
  }
};

class PointCollector : public CurveOperator {
public:
  Point2DArray m_result;
  void apply(const Point2D &p) override {
    m_result.add(p);
  }
  void line(const Point2D &from, const Point2D &to) override {
  };
};

void applyToLine(          int x1, int y1, int x2, int y2,              PointOperator &op);
void applyToLine(          const CPoint &p1, const CPoint &p2,          PointOperator &op);
void applyToRectangle(     const CRect &rect,                           PointOperator &op);
void applyToFullRectangle( const CRect &rect,                           PointOperator &op);
void applyToEllipse(       const CRect &rect,                           PointOperator &op);
// call for curve.m_type == TT_PRIM_LINE
// Return last point
Point2D applyToPrimLine(                         const Point2DArray &a, CurveOperator &op);
// call for curve.m_type == TT_PRIM_CSPLINE
// Return last point
Point2D applyToPrimCSpline(const Point2D &start, const Point2DArray &a, CurveOperator &op);
void applyToBezier(        const Point2D &start, const Point2D &cp1, const Point2D &cp2, const Point2D &end, CurveOperator &op, bool applyStart=true);
