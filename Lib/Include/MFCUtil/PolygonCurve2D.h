#pragma once

#include <Math/Rectangle2D.h>
#include "PolygonCurveBase.h"

template<typename PointType> class PolygonCurve2DTemplate : public _PolygonCurveTypeName {
public:
  short                           m_type; // TT_PRIM_LINE, TT_PRIM_QSPLINE or TT_PRIM_CSPLINE
  PointArrayTemplate<PointType,2> m_points;
  PolygonCurve2DTemplate(short type=0) {
    m_type = type;
  }
  template<typename T> void addPoint(const FixedSizeVectorTemplate<T, 2> &v) {
    m_points.add(v);
  }

  inline const PointArrayTemplate<PointType,2> &getAllPoints() const {
    return m_points;
  }
  inline PointArrayTemplate<PointType,2> &getAllPoints() {
    return m_points;
  }
  template<typename T> void move(const FixedSizeVectorTemplate<T, 2> &v) {
    for(size_t i = 0; i < m_points.size(); i++) {
      m_points[i] += v;
    }
  }
  inline short getType() const {
    return m_type;
  }
  String toString() const {
    return format(_T("%s\n"), typeToStr(m_type)) + indentString(m_points.toString(_T("\n")),4);
  }
};

class PolygonCurve2D : public PolygonCurve2DTemplate<Point2D> {
public:
  PolygonCurve2D(short type=0) : PolygonCurve2DTemplate(type) {
  }
  Rectangle2D getBoundingBox() const {
    Rectangle2D result;
    return m_points.getBoundingBox(result);
  }
};
