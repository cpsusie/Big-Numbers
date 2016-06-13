#include "pch.h"
#include <Math/Polygon2D.h>

void Polygon2D::move(const Point2D &dp) {
  for(size_t i = 0; i < size(); i++) {
    (*this)[i] += dp;
  }
}

Rectangle2D Polygon2D::getBoundingBox() const {
  const Point2DArray &a = *this;
  double minx,maxx,miny,maxy;
  for(size_t i = 0; i < a.size(); i++) {
    const Point2D &p = a[i];
    if(i == 0) {
      minx = maxx = p.x;
      miny = maxy = p.y;
    } else {
      if(p.x < minx) minx = p.x;
      if(p.x > maxx) maxx = p.x;
      if(p.y < miny) miny = p.y;
      if(p.y > maxy) maxy = p.y;
    }
  }
  return Rectangle2D(minx,miny,maxx-minx,maxy-miny);
}

int Polygon2D::contains(const Point2D &p) const { // 1=inside, -1=outside, 0=edge
  unsigned int n = (int)size();
  if(n < 3) return -1;
  Point2D v = (*this)[0] - p;
  if(v.x == 0 && v.y == 0) {
    return 0; // edge
  }
  double d = 0;
  for(unsigned int i = 1; i <= n; i++) {
    const Point2D vnext = (*this)[i%n] - p;
    if(vnext.x == 0 && vnext.y == 0) {
      return 0; // edge
    }
    d += angle(v, vnext);
    v = vnext;
  }
  return fabs(d) > 1 ? 1 : -1; // d always +/- 2PI or 0.
}
