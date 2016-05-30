#pragma once

#include "C3D.h"

class Spherical {
private:
  void init(double x, double y, double z);
public:
  double r, fi, theta; // r distance to (0,0,0), fi angle between z-axis and (x,y,z), theta angle between x-axis and (x,y,z)
                       // 0 <= r, 0 <= fi <= pi, 0 <= theta <= 2pi. theta > pi >= y < 0
  Spherical();
  Spherical(const D3DVECTOR &v);
  Spherical(double x, double y, double z);
  operator D3DVECTOR() const;
  String toString(int dec = 1, bool rad = false) const;
};
