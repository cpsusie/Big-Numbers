#include "stdafx.h"
#include "DrawToolPolygon.h"

bool DrawToolPolygon::OnLButtonDown(UINT nFlags, CPoint point) {
  Point2D p        = m_editor.getViewport().backwardTransform(point);
  Profile2D &profile = m_editor.getProfile();
  if(m_pp == nullptr) {
    ProfilePolygon2D pp;
    pp.m_start  = p;
    pp.m_closed = false;
    ProfileCurve2D curve(TT_PRIM_LINE);
    curve.addPoint(p);
    pp.addCurve(curve);
    profile.addPolygon(pp);
    m_pp = &profile.getLastPolygon();
  } else {
    m_pp->getLastCurve().addPoint(p);
    repaintAll();
  }
  return true;
}

bool DrawToolPolygon::OnLButtonDblClk(UINT nFlags, CPoint point) {
  if(m_pp != nullptr) {
    m_pp->m_closed = true;
    m_pp->removeLastPoint();
  }
  m_pp = nullptr;
  repaintAll();
  return true;
}

bool DrawToolPolygon::OnMouseMove(UINT nFlags, CPoint point) {
  Point2D p = m_editor.getViewport().backwardTransform(point);
  if(m_pp != nullptr) {
    m_pp->getLastPoint() = p;
    repaintAll();
    return true;
  }
  return false;
}

bool DrawToolPolygon::OnLButtonUp(UINT nFlags, CPoint point) {
  return true;
}
