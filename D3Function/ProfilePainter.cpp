#include "stdafx.h"
#include <MFCUtil/Viewport2D.h>
#include <MFCUtil/ShapeFunctions.h>
#include <D3DGraphics/Profile.h>
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

void paintProfilePolygon(const ProfilePolygon &pp, Viewport2D &vp, COLORREF color) {
  pp.apply(ProfilePainter(vp,color));
}

void paintProfile(const Profile &profile, Viewport2D &vp, COLORREF color) {
  profile.apply(ProfilePainter(vp,color));
}

void paintProfileNormals(const Profile &profile, Viewport2D &vp, COLORREF color, bool smooth) {
  Vertex2DArray va = profile.getAllVertices(smooth);
  const size_t  n  = va.size();
  CPen          pen;
  pen.CreatePen(BS_SOLID, 1, color);
  vp.SelectObject(&pen);
  CDC &dc = *vp.getDC();
  for(size_t i = 0; i < n; i++) {
    const Vertex2D     &v      = va[i];
    const FloatPoint2D &from   = v.m_pos;
    const FloatPoint2D &normal = v.m_normal;
    const FloatPoint2D &to     = from + normal;

    vp.MoveTo(from);
    Point2D tfrom   = vp.forwardTransform(from);
    Point2D tto     = vp.forwardTransform(to);
    Point2D tnormal = tto-tfrom;
    double  tlen    = tnormal.length();
    tnormal /= tlen;
    tto = tfrom + tnormal * 20;
    dc.LineTo((Point2DP)tto);
    dc.LineTo((Point2DP)(tto + tnormal.rotate(GRAD2RAD(160)) * 5));
    dc.MoveTo((Point2DP)tto);
    dc.LineTo((Point2DP)(tto + tnormal.rotate(GRAD2RAD(-160)) * 5));
  }
}
