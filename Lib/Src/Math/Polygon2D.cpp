#include "pch.h"
#include <Math/Polygon2D.h>

Rectangle2D Polygon2D::getBoundingBox() const {
  Rectangle2D result;
  return __super::getBoundingBox(result);
}

int Polygon2D::contains(const Point2D &p) const { // 1=inside, -1=outside, 0=edge
  UINT n = (UINT)size();
  if(n < 3) return -1;
  Point2D v = (*this)[0] - p;
  if(v.x() == 0 && v.y() == 0) {
    return 0; // edge
  }
  double d = 0;
  for(UINT i = 1; i <= n; i++) {
    const Point2D vnext = (*this)[i%n] - p;
    if(vnext.x() == 0 && vnext.y() == 0) {
      return 0; // edge
    }
    d += angle2D(v, vnext);
    v = vnext;
  }
  return fabs(d) > 1 ? 1 : -1; // d always +/- 2PI or 0.
}
