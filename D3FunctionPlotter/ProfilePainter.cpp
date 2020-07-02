#include "stdafx.h"
#include "ProfilePainter.h"

class ProfilePainter : public CurveOperator {
  Viewport2D &m_vp;
  CPen      m_pen;
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

class NormalPainter : public CurveOperator {
private:
  Viewport2D          &m_vp;
  CPen                 m_pen;
protected:
  const Point2DArray  &m_normals;
  int                  m_count;
  void drawNormal(const Point2D &from, const Point2D &dir);
public:
  NormalPainter(Viewport2D &vp, const Point2DArray &normals, COLORREF color);
};

NormalPainter::NormalPainter(Viewport2D &vp, const Point2DArray &normals, COLORREF color) : m_vp(vp), m_normals(normals) {
  m_count = 0;
  m_pen.CreatePen(BS_SOLID,1,color);
  m_vp.SelectObject(&m_pen);
}

void NormalPainter::drawNormal(const Point2D &from, const Point2D &dir) {
  m_vp.MoveTo(from);
  Point2D from2 = m_vp.forwardTransform(from);
  Point2D to2   = m_vp.forwardTransform(from+dir);
  Point2D dir2 = to2 - from2;
  double length  = dir2.length();
  dir2 /= length;
  to2 = from2 + dir2 * 20;
  m_vp.getDC()->LineTo(to2);
  m_vp.getDC()->LineTo(to2 + dir2.rotate(GRAD2RAD(160)) * 5);
  m_vp.getDC()->MoveTo(to2);
  m_vp.getDC()->LineTo(to2 + dir2.rotate(GRAD2RAD(-160)) * 5);
}

class FlatNormalPainter : public NormalPainter {
public:
  void line(const Point2D &from, const Point2D &to);
  FlatNormalPainter(Viewport2D &vp, const Point2DArray &normals, COLORREF color);
};

FlatNormalPainter::FlatNormalPainter(Viewport2D &vp, const Point2DArray &normals, COLORREF color) : NormalPainter(vp,normals,color) {
}

void FlatNormalPainter::line(const Point2D &from, const Point2D &to) {
  Point2D mid = (from + to)/2;
  drawNormal(mid,m_normals[m_count++]);
}

class SmoothNormalPainter : public NormalPainter {
public:
  void line(const Point2D &from, const Point2D &to) {};
  void apply(const Point2D &p);
  SmoothNormalPainter(Viewport2D &vp, const Point2DArray &normals, COLORREF color);
};

SmoothNormalPainter::SmoothNormalPainter(Viewport2D &vp, const Point2DArray &normals, COLORREF color) : NormalPainter(vp,normals,color) {
}

void SmoothNormalPainter::apply(const Point2D &p) {
  if(m_count >= (int)m_normals.size())
    return;
  drawNormal(p,m_normals[m_count++]);
}

void paintProfilePolygon(const ProfilePolygon &pp, Viewport2D &vp, COLORREF color) {
  pp.apply(ProfilePainter(vp,color));
}

void paintProfile(const Profile &profile, Viewport2D &vp, COLORREF color) {
  profile.apply(ProfilePainter(vp,color));
}

void paintProfileNormals(const Profile &profile, Viewport2D &vp, COLORREF color, bool smooth) {
  if(smooth) {
    for(size_t i = 0; i < profile.m_polygonArray.size(); i++) {
      const ProfilePolygon &pp = profile.m_polygonArray[i];
      pp.apply(SmoothNormalPainter(vp,pp.getSmoothNormals(),color));
    }
  }
  else {
    profile.apply(FlatNormalPainter(vp,profile.getFlatNormals(),color));
  }
}

