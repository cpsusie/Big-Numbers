#include "pch.h"
#include <Math/Rectangle2D.h>

Rectangle2D getUnion(const Rectangle2D &r1, const Rectangle2D &r2) {
  const double minX = min(r1.getMinX(), r2.getMinX());
  const double minY = min(r1.getMinY(), r2.getMinY());
  const double maxX = max(r1.getMaxX(), r2.getMaxX());
  const double maxY = max(r1.getMaxY(), r2.getMaxY());
  return Rectangle2D(minX,minY, maxX-minX,maxY-minY);
}

Rectangle2D Point2DArray::getBoundingBox() const {
  if(size() == 0) {
    return Rectangle2D();
  }

  const Point2D *p = &first();
  double minX=p->x, maxX=minX, minY=p->y, maxY=minY;
  p++;
  for(size_t i = size(); --i; p++) {
    if(p->x < minX) minX = p->x; else if(p->x > maxX) maxX = p->x;
    if(p->y < minY) minY = p->y; else if(p->y > maxY) maxY = p->y;
  }
  return Rectangle2D(minX,minY, maxX-minX, maxY-minY);
}

String Point2DRefArray::toString(int dec) const {
  String result = _T("(");
  if(size()) {
    const Point2D * const *p = &first();
    result += (*(p++))->toString(dec);
    for(size_t i = size()-1; i--;) {
      result += _T(",");
      result += (*(p++))->toString(dec);
    }
  }
  result += _T(")");
  return result;
}
