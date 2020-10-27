#include "stdafx.h"
#include "DrawToolLine.h"

bool DrawToolLine::OnLButtonDown(UINT nFlags, CPoint point) {
  Profile2D &profile = m_editor.getProfile();
  Point2D p = m_editor.getViewport().backwardTransform(point);
  profile.addLine(p,p);
  ProfilePolygon2D &gp = profile.m_polygonArray.last();
  m_p0 = &gp.m_start;
  m_p1 = &(Point2D&)gp.m_curveArray.last().m_points.last();
  unselectAll();
  select(&gp).repaintAll();
  return true;
}

bool DrawToolLine::OnMouseMove(UINT nFlags, CPoint point) {
  if(nFlags & MK_LBUTTON && m_p1 != nullptr) {
    *m_p1 = m_editor.getViewport().backwardTransform(point);
    repaintAll();
    return true;
  }
  return false;
}

bool DrawToolLine::OnLButtonUp(UINT nFlags, CPoint point) {
  m_p0 = m_p1 = nullptr;
  return true;
}
