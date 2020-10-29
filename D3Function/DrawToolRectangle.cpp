#include "stdafx.h"
#include "DrawToolRectangle.h"

bool DrawToolRectangle::OnLButtonDown(UINT nFlags, CPoint point) {
  Profile2D       &profile = m_editor.getProfile();
  const Point2D    p       = m_editor.getViewport().backwardTransform(point);
  ProfilePolygon2D polygon;
  ProfileCurve2D   curve(TT_PRIM_LINE);
  polygon.m_start  = p;
  polygon.m_closed = true;
  curve.addPoint(p);
  curve.addPoint(p);
  curve.addPoint(p);
  polygon.addCurve(curve);
  profile.addPolygon(polygon);

  ProfilePolygon2D &gp = profile.m_polygonArray.last();
  ProfileCurve2D   &pc = gp.m_curveArray.last();
  m_ul = &gp.m_start;
  m_ur = &pc.m_points[0];
  m_lr = &pc.m_points[1];
  m_ll = &pc.m_points[2];

  unselectAll();
  select(&gp);
  repaintAll();
  return true;
}

bool DrawToolRectangle::OnMouseMove(UINT nFlags, CPoint point) {
  if(nFlags & MK_LBUTTON) {
    const Point2D p = m_editor.getViewport().backwardTransform(point);
    m_ur->x() = p.x();
    *m_lr = p;
    m_ll->y() = p.y();
    repaintAll();
    return true;
  }
  return false;
}

bool DrawToolRectangle::OnLButtonUp(UINT nFlags, CPoint point) {
  repaintScreen();
  return true;
}
