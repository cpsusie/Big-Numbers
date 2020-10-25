#include "stdafx.h"
#include "DrawToolBezierCurve.h"

bool DrawToolBezierCurve::OnLButtonDown(  UINT nFlags, CPoint point) {
  Profile2D &profile = m_editor.getProfile();
  Point2D p = m_editor.getViewport().backwardTransform(point);
  ProfilePolygon2D polygon;
  ProfileCurve2D   curve(TT_PRIM_CSPLINE);
  polygon.m_start  = p;
  polygon.m_closed = false;
  p.x() += 0.1;
  p.y() += 0.1;
  curve.addPoint(p);
  p.x() += 0.1;
  curve.addPoint(p);
  p.x() += 0.1;
  p.y() -= 0.1;
  curve.addPoint(p);
  polygon.addCurve(curve);
  profile.addPolygon(polygon);

  unselectAll();
  select(&profile.m_polygonArray.last());
  repaintAll();
  return true;
}

bool DrawToolBezierCurve::OnMouseMove(    UINT nFlags, CPoint point) {
  return true;
}

bool DrawToolBezierCurve::OnLButtonUp(    UINT nFlags, CPoint point) {
  return true;
}
