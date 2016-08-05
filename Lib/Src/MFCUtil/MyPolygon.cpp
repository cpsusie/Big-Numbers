#include "stdafx.h"
#include <MFCUtil/PixRect.h>

class PolygonFiller : public PixRectOperator {
private:
  MyPolygon &m_polygon;
  void checkAndFill(const CPoint &p);
public:
  PointArray m_pointsOutside;
  PolygonFiller(PixRect *pixRect, MyPolygon &polygon);
  void apply(const CPoint &p);
  void restoreBlackOutSideRegion();
};

PolygonFiller::PolygonFiller(PixRect *pixRect, MyPolygon &polygon) 
: m_polygon(polygon) {
  setPixRect(pixRect);
}

// Fills BLACK inside polygon, To speed up edge-scanner we fill RED outside, and collect all points used initially to fill 
// these regions. They are afterwards removed (filled whith BLACK) from mask, so mask finally contains WHITE inside (and edge),
// and BLACK outside.
void PolygonFiller::checkAndFill(const CPoint &p) {
  if(!m_pixRect->contains(p)) {
    return;
  }
  if(m_pixelAccessor->getPixel(p) == 0)
    switch(m_polygon.contains(p)) {
    case 1:
      m_pixelAccessor->fill(p,WHITE);
      break;
    case -1:
      m_pixelAccessor->fill(p,D3DCOLOR_XRGB(255,0,0));
      m_pointsOutside.add(p);
      break;
    case 0:
      break;
  }
}

void PolygonFiller::restoreBlackOutSideRegion() {
  for(size_t i = 0; i < m_pointsOutside.size(); i++) {
    m_pixelAccessor->fill(m_pointsOutside[i],BLACK);
  }
}

void PolygonFiller::apply(const CPoint &p) {
  checkAndFill(CPoint(p.x-1,p.y));
  checkAndFill(CPoint(p.x+1,p.y));
  checkAndFill(CPoint(p.x,p.y-1));
  checkAndFill(CPoint(p.x,p.y+1));
}

void PixRect::fillPolygon(const MyPolygon &polygon, D3DCOLOR color, bool invert) {
  CRect rect = polygon.getBoundsRect();
  MyPolygon poly = polygon;
  poly.move(-rect.TopLeft());
  PixRect *psrc  = new PixRect(m_device, getType(), rect.Size(), getPool(), getPixelFormat());
  PixRect *pmask = new PixRect(m_device, getType(), rect.Size(), getPool(), getPixelFormat());
  psrc->fillRect(0,0,rect.Width(),rect.Height(),color);
  pmask->fillRect(0,0,rect.Width(),rect.Height(),0);   // set mask to black
  pmask->polygon(poly,WHITE);                          // draw white frame around polygon on mask

  PolygonFiller *polygonFiller = new PolygonFiller(pmask, poly);
  poly.applyToEdge(*polygonFiller);
  polygonFiller->restoreBlackOutSideRegion();
  delete polygonFiller;

//  rop(rect.left,rect.top,rect.Width(),rect.Height(),DSTINVERT,NULL,0,0);
//  mask(rect.left,rect.top,rect.Width(),rect.Height(), MAKEROP4(SRCCOPY,DSTINVERT), psrc, 0,0, pmask);
  mask(rect.left,rect.top,rect.Width(),rect.Height(), SRCCOPY, psrc, 0,0, pmask);
  delete pmask;
  delete psrc;
}

void MyPolygon::move(const CPoint &dp) {
  for(size_t i = 0; i < size(); i++) {
    (*this)[i] += dp;
  }
}

CRect MyPolygon::getBoundsRect() const {
  const PointArray &a = *this;
  const size_t      n = a.size();
  if(n == 0) {
    return CRect(0,0,0,0);
  }
  CRect result(a[0],a[0]);
  for(size_t i = 1; i < n; i++) {
    const CPoint &p = a[i];
    if(p.x < result.left  ) result.left   = p.x;
    if(p.x > result.right ) result.right  = p.x;
    if(p.y < result.top   ) result.top    = p.y;
    if(p.y > result.bottom) result.bottom = p.y;
  }
  return CRect(result.left,result.top,result.right+1,result.bottom+1);
}

class Vector : public CPoint {
public:
  Vector(const CPoint &from, const CPoint &to);
  double length() const;
};

Vector::Vector(const CPoint &from, const CPoint &to) {
  x = to.x - from.x;
  y = to.y - from.y;
}

double operator*(const Vector &v1, const Vector &v2) {
  return v1.x*v2.x + v1.y*v2.y;
}

double Vector::length() const {
  return sqrt(sqr(x) + sqr(y));
}

static int det(const Vector &v1, const Vector &v2) {
  return v1.x*v2.y - v1.y*v2.x;
}

static double angle(const Vector &v1, const Vector &v2) {
  if(v1 == v2) {
    return 0;
  }
  double a = acos(v1*v2 / v1.length() / v2.length());
  return a * sign(det(v1,v2));
}

bool MyPolygon::add(const CPoint &p) {
  if(size() > 0 && p == last()) {
    return false;
  }
  PointArray::add(p);
  return true;
}

int MyPolygon::contains(const CPoint &p) const { // 1=inside, -1=outside, 0=edge
  size_t n = size();
  if(n < 3) {
    return -1;
  }
  Vector v(p,(*this)[0]);
  if((v.x == 0) && (v.y == 0)) {
    return 0; // edge
  }
  double d = 0;
  for(size_t i = 1; i <= n; i++) {
    Vector vnext(p,(*this)[i%n]);
    if((vnext.x == 0) && (vnext.y == 0)) {
      return 0; // edge
    }
    d += angle(v,vnext);
    v = vnext;
  }
  return fabs(d) > 1 ? 1 : -1; // d always +/- 2PI or 0.
}

class PolygonEdgeOperator : public PointOperator {
  PointOperator &m_op;
  const CPoint  &m_endPoint;
public:
  PolygonEdgeOperator(PointOperator &op, const CPoint &endPoint);
  void apply(const CPoint &p);
};

PolygonEdgeOperator::PolygonEdgeOperator(PointOperator &op, const CPoint &endPoint) : m_op(op), m_endPoint(endPoint) {
}

void PolygonEdgeOperator::apply(const CPoint &p) {
  if(p != m_endPoint) {
    m_op.apply(p);
  }
}

void MyPolygon::applyToEdge(PointOperator &op, bool closingEdge) const {
  size_t n = size();
  for(size_t i = 1; i < n; i++) {
    const CPoint &p1 = (*this)[i-1];
    const CPoint &p2 = (*this)[i];
    applyToLine(p1,p2,PolygonEdgeOperator(op,p2));
  }
  if(closingEdge && (n > 2)) {
    const CPoint &p1 = last();
    const CPoint &p2 = (*this)[0];
    applyToLine(p1,p2,PolygonEdgeOperator(op,p2));
  }
}
