#pragma once

#include "Point3D.h"

class Spherical {
protected:
  void init(double x, double y, double z);
public:
  double r, fi, theta; // r distance to (0,0,0), fi angle between z-axis and (x,y,z), theta angle between x-axis and (x,y,z)
                       // 0 <= r, 0 <= fi <= pi, 0 <= theta <= 2pi. theta > pi >= y < 0
  Spherical();
  Spherical(const Point3D &p);
  Spherical(double x, double y, double z);
  operator Point3D() const;
  String toString(int dec = 1, bool rad = false) const;
};
