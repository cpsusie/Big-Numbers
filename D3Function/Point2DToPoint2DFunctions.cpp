#include "stdafx.h"
#include <Math.h>
#include "Point2DToPoint2DFunctions.h"

// ----------------------- MoveTransformation -----------------------
MoveTransformation::MoveTransformation(const Point2D &dp) {
  m_dp = dp;
}

Point2D MoveTransformation::operator()(const Point2D &p) {
  return p + m_dp;
}

// ----------------------- RectAreaTransformation ----------------------
RectAreaTransformation::RectAreaTransformation(const Point2D &origin, const Point2D &u1, const Point2D &u2) {
  m_origin  = origin;
  m_u1      = unitVector(u1);
  m_u2      = unitVector(u2);
  m_det     = det(m_u1, m_u2);
}

Point2D RectAreaTransformation::toBaseU(const Point2D &p) {
  double k1 = det(p,m_u2)/m_det;
  double k2 = det(m_u1,p)/m_det;
  return Point2D(k1,k2);
}

Point2D RectAreaTransformation::toView(const Point2D &k) {
  return k.x()*m_u1 + k.y()*m_u2;
}

// ----------------------- StretchTransformation -----------------------
StretchTransformation::StretchTransformation(const Point2D &origin, const Point2D &u1, const Point2D &u2, const Point2D &p0, const Point2D &step)
: RectAreaTransformation(origin,u1,u2) {
  m_k0      = toBaseU(p0-getOrigin());
  m_step    = toBaseU(step);
}

#define EPS 1e-8

Point2D StretchTransformation::operator()(const Point2D &p) {
  Point2D k     = toBaseU(p-getOrigin());
  Point2D rstep = m_step;

  rstep.x() *= (fabs(m_k0.x())<EPS)?0:(k.x() / m_k0.x());
  rstep.y() *= (fabs(m_k0.y())<EPS)?0:(k.y() / m_k0.y());

  return p + toView(rstep);
}

// ----------------------- SkewTransformation -----------------------

extern char infostr[400];

SkewTransformation::SkewTransformation(const Point2D &origin, const Point2D &u1, const Point2D &u2, const Point2D &p0, const Point2D &step)
: RectAreaTransformation(origin,u1,u2) {
  m_k0      = toBaseU(p0-getOrigin());
  m_step    = toBaseU(step);
/*
  Point2D stepV   = toView(m_step);

  sprintf(infostr,"orig:(%le,%le)\nu1:(%le,%le)\nu2:(%le,%le)\nstep:(%le,%le)\nm_step:(%le,%le)\nstepV:(%le,%le)\np0:(%le,%le)\nk0:(%le,%le)",
    getOrigin().x,getOrigin().y,getU1().x,getU1().y,getU2().x,getU2().y,step.x,step.y,m_step.x,m_step.y,stepV.x,stepV.y,p0.x,p0.y,m_k0.x,m_k0.y);
*/
}

Point2D SkewTransformation::operator()(const Point2D &p) {
  Point2D k = toBaseU(p-getOrigin());
  Point2D rstep;

  if(fabs(m_k0.y()) > fabs(m_k0.x())) {
    rstep.x() = m_step.x() * k.y() / m_k0.y();
    rstep.y() = 0;
  } else {
    rstep.x() = 0;
    rstep.y() = m_step.y() * k.x() / m_k0.x();
  }
  return p + toView(rstep);
}


// ----------------------- RotateTransformation -----------------------
RotateTransformation::RotateTransformation(const Point2D &center, double theta) {
  m_center = center;
  m_theta  = theta;
}

Point2D RotateTransformation::operator()(const Point2D &p) {
  return rotate(Point2D(p - m_center),m_theta) + m_center;
}

// ----------------------- MirrorTransformation -----------------------
MirrorTransformation::MirrorTransformation(const Point2D &p1, const Point2D &p2) {
  m_p1 = p1;
  m_p2 = p2;
}

Point2D MirrorTransformation::operator()(const Point2D &p) {
  const Point2D u      = unitVector(m_p2 - m_p1);
  const Point2D v      = p - m_p1;
  const Point2D prj    = (u * v) * u;
  const Point2D normal = v - prj;
  return m_p1 + (prj - normal);
}
