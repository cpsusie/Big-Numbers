#include "stdafx.h"
#include <MFCUtil/Viewport2D.h>
#include <MFCUtil/ShapeFunctions.h>
#include <D3DGraphics/Profile2D.h>
#include "ProfilePainter.h"

class ProfilePainter : public CurveOperator {
  Viewport2D &m_vp;
  CPen        m_pen;
public:
  void line(const Point2D &from, const Point2D &to);
  ProfilePainter(Viewport2D &vp, COLORREF color);
};

ProfilePainter::ProfilePainter(Viewport2D &vp, COLORREF color) : m_vp(vp) {
  m_pen.CreatePen(BS_SOLID,1,color);
  m_vp.SelectObject(&m_pen);
}

void ProfilePainter::line(const Point2D &from, const Point2D &to) {
  m_vp.MoveTo(from);
  m_vp.LineTo(to);
}

void paintProfilePolygon(const ProfilePolygon2D &pp, Viewport2D &vp, COLORREF color) {
  pp.apply(ProfilePainter(vp,color));
}

void paintProfile(const Profile2D &profile, Viewport2D &vp, COLORREF color) {
  profile.apply(ProfilePainter(vp,color));
}

void paintArrow(CDC &dc, const Point2D &p, const Point2D &dir, double length) {
  const Point2D udir = unitVector(dir);
  const Point2D end  = p + udir * length;
  dc.MoveTo(p);
  dc.LineTo(end);
  dc.LineTo(end + udir.rotate(GRAD2RAD( 160)) * length/4);
  dc.MoveTo(end);
  dc.LineTo(end + udir.rotate(GRAD2RAD(-160)) * length/4);
}

void paintProfileNormals(const Profile2D &profile, Viewport2D &vp, COLORREF color, bool smooth) {
  const Vertex2DArray va = profile.getAllVertices(smooth);
  CPen                pen;
  pen.CreatePen(BS_SOLID, 1, color);
  vp.SelectObject(&pen);
  CDC &dc = *vp.getDC();
  for(const Vertex2D v : va) {
    const Point2D from = vp.forwardTransform(v.m_pos);
    const Point2D dir  = vp.forwardTransform(v.m_pos + v.m_normal) - from;
    paintArrow(dc, from, dir, 20);
  }
}
