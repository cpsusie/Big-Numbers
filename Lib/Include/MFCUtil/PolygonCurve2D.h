#pragma once

#include <Math/Rectangle2D.h>

typedef struct {
  const short  m_type;
  const TCHAR *m_name;
} PolygonCurveTypeName;

class _PolygonCurveTypeName {
private:
  static const PolygonCurveTypeName s_typeNames[4];
public:
  // throws exception if type not in {0,TT_PRIM_LINE, TT_PRIM_QSPLINE, TT_PRIM_CSPLINE}. type==0 returns ""
  static const TCHAR *typeToStr(short        type);
  // throws exception if str not in {"","line","qspline", "cspline"} (comparing ignore case) str=="" return 0
  static short        strToType(const TCHAR *str );
};

template<typename T> class PolygonCurve2DTemplate : public _PolygonCurveTypeName {
public:
  short                   m_type; // TT_PRIM_LINE, TT_PRIM_QSPLINE or TT_PRIM_CSPLINE
  Point2DTemplateArray<T> m_points;
  template<typename P> void addPoint(const Point2DTemplate<P> &p) {
    m_points.add(p);
  }

  PolygonCurve2DTemplate(short type=0) {
    m_type = type;
  }
  template<typename P> PolygonCurve2DTemplate(const PolygonCurve2DTemplate<P> &src)
    : m_type(src.m_type)
    , m_points(src.m_points)
  {
  }

  Rectangle2DTemplate<T> getBoundingBox() const {
    return m_points.getBoundingBox();
  }
  inline const Point2DTemplateArray<T> &getAllPoints() const {
    return m_points;
  }
  template<typename P> void move(const Point2DTemplate<P> &dp) {
    for(size_t i = 0; i < m_points.size(); i++) {
      m_points[i] += dp;
    }
  }
  inline short getType() const {
    return m_type;
  }
  String toString() const {
    return format(_T("%s\n"), typeToStr(m_type)) + indentString(m_points.toString(_T("\n")),4);
  }
};

class PolygonCurve2D : public PolygonCurve2DTemplate<double> {
public:
  PolygonCurve2D(short type=0) : PolygonCurve2DTemplate(type) {
  }
  template<typename P> PolygonCurve2D(const PolygonCurve2DTemplate<P> &src) : PolygonCurve2DTemplate<double>(src) {
  }
};
