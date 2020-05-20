#include "pch.h"
#include <MFCUtil/PolygonCurve.h>

// ------------------------------------ PolygonCurve ------------------------------

void PolygonCurve::move(const Point2D &dp) {
  for(size_t i = 0; i < m_points.size(); i++) {
    m_points[i] += dp;
  }
}

Rectangle2D PolygonCurve::getBoundingBox() const {
  return m_points.getBoundingBox();
}

String PolygonCurve::toString() const {
  String result;
  switch(m_type) {
  case TT_PRIM_LINE   : result = _T("line   :"); break;
  case TT_PRIM_QSPLINE: result = _T("qspline:"); break;
  case TT_PRIM_CSPLINE: result = _T("cspline:"); break;
  default             : result = format(_T("unknown type:%d:"), m_type); break;
  }

  const TCHAR *delim = EMPTYSTRING;
  for(size_t i = 0; i < m_points.size(); i++, delim = _T("        ")) {
    result += format(_T("%s%s\n"), delim, m_points[i].toString().cstr());
  }
  return result;
}
