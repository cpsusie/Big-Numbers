#pragma once

#include "Point2D.h"
#include "Rectangle2D.h"

class Polygon2D : public Point2DArray {
public:
  void move(const Point2D &dp);
  Rectangle2D getBoundingBox() const;
  int contains(const Point2D &p) const; // 1=inside, -1=outside, 0=edge
};

