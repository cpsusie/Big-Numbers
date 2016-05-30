#include "pch.h"
#include <Math/Point3D.h>

double angle(const Point3D &p1, const Point3D &p2) {
  const double l1 = p1.length();
  const double l2 = p2.length();
  if((l1 == 0) || (l2 == 0)) {
    return 0;
  } else {
    const double f = (p1 * p2) / (l1 * l2);
    if(f <= -1) {
      return M_PI;
    } else if(f >= 1) {
      return 0;
    } else {
//      debugLog("l1:%le, l2:%le, p1:%s, p2:%s\n", l1, l2, p1.toString(10).cstr(), p2.toString(10).cstr());
      return acos(f);
    }
  }
}

String Point3D::toString(int dec) const {
  const int n = dec+3;
  return format(_T("(% *.*f,% *.*f,% *.*f)"), n,dec,x, n,dec,y, n,dec,z);
}
