#pragma once

#include "Rectangle2D.h"

class Polygon2D : public Point2DArray {
public:
  template<typename T> Polygon2D &move(const FixedSizeVectorTemplate<T, 2> &v) {
    (*this) += v;
    return *this;
  }
  Rectangle2D getBoundingBox() const;
  int contains(const Point2D &p) const; // 1=inside, -1=outside, 0=edge
};

