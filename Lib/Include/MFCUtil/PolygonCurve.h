#pragma once

#include <Math/Rectangle2D.h>

class PolygonCurve {
private:
  short          m_type; // TT_PRIM_LINE, TT_PRIM_QSPLINE or TT_PRIM_CSPLINE
  Point2DArray   m_points;
public:
  void addPoint(const Point2D &p) {
    m_points.add(p);
  }

  PolygonCurve(short type) {
    m_type = type;
  }

  Rectangle2D getBoundingBox() const;
  inline const Point2DArray &getAllPoints() const {
    return m_points;
  }
  void move(const Point2D &dp);
  inline short getType() const {
    return m_type;
  }
  String toString() const;
  String toXML();
};
