#include "pch.h"
#include <Math/Point2D.h>

Point2D::Point2D(const String &s) {
  DEFINEMETHODNAME;
  const _TUCHAR *cp = (_TUCHAR*)s.cstr();
  while(_istspace(*cp)) cp++;
  bool gotParentes = false;
  if(*cp == '(') {
    cp++;
    gotParentes = true;
  }
  const _TUCHAR *cpx = cp;
  cp = parseReal(cp);
  if(cp == NULL) {
    throwInvalidArgumentException(method, _T("s=%s"), s.cstr());
  }
  while(_istspace(*cp)) cp++;
  switch(*cp) {
  case _T(','):
  case _T(';'):
    cp++;
    break;
  }
  const _TUCHAR *cpy = cp;
  cp = parseReal(cp);
  if(cp == NULL) {
    throwInvalidArgumentException(method, _T("s=%s"), s.cstr());
  }
  if(gotParentes) {
    while(_istspace(*cp)) cp++;
    if(*cp != _T(')')) {
      throwInvalidArgumentException(method, _T("s=%s. missing ')'"), s.cstr());
    }
  }
  x = _ttof(cpx);
  y = _ttof(cpy);
}

double angle(const Point2D &p1, const Point2D &p2) {  // angle in radians between p1 and p2
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
      return acos(f) * sign(det(p1, p2));
    }
  }
}

Point2D Point2D::rotate(double rad) const {
  const double cs = cos(rad);
  const double sn = sin(rad);
  Point2D result;
  result.x = cs * x + -sn * y;
  result.y = sn * x + cs * y;
  return result;
}

Point2D pointOfIntersection(const Line2D &line1, const Line2D &line2, bool &intersect) {
  const double A11 = line1.m_p1.y       - line1.m_p2.y;
  const double A12 = line1.m_p2.x       - line1.m_p1.x;
  const double A21 = line2.m_p1.y       - line2.m_p2.y;
  const double A22 = line2.m_p2.x       - line2.m_p1.x;

  const double d  = A11 * A22 - A12 * A21;
  if (d == 0) {
    intersect = false;
    return Point2D(0,0);
  }
  intersect = true;

  const double B1  = line1.m_p1.x * A11 + line1.m_p1.y * A12;
  const double B2  = line2.m_p1.x * A21 + line2.m_p1.y * A22;

//  A11*x + A12*y = B1
//  A21*x + A22*y = B2
// d = | A11 A12 |  dx = | B1 A12 |  dy = | A11 B1 |
//     | A21 A22 |       | B2 A22 |       | A21 B2 |
// Cramers rule: (x,y) = (dx/d, dy/d)

  const double dx = B1  * A22 - B2  * A12;
  const double dy = A11 * B2  - A21 * B1;
  return Point2D(dx/d, dy/d);
}

double distanceFromLine(const Line2D &line, const Point2D &p) {
  return distanceFromLine(line.m_p1, line.m_p2, p);
}

double distanceFromLineSegment(const Line2D &line, const Point2D &p) {
  return distanceFromLineSegment(line.m_p1, line.m_p2,p);
}

double distanceFromLine(const Point2D &lp0, const Point2D &lp1, const Point2D &p) {
  if(lp1 == lp0) {
    return distance(p,lp1);
  }
  Point2D u = lp1 - lp0;
  const Point2D d = p-lp0;
  u /= u.length();
  return (d - (d * u) * u).length();
}

double distanceFromLineSegment(const Point2D &lp0, const Point2D &lp1, const Point2D &p) {
  const Point2D u = lp1 - lp0;
  if((p-lp0) * u < 0) {
    return distance(p,lp0);
  } else if((p-lp1) * u > 0) {
    return distance(p,lp1);
  } else {
    return distanceFromLine(lp0,lp1,p);
  }
}

String Point2D::toXML() const {
  return format(_T("<point>\n<x>%lf</x>\n<y>%lf</y>\n</point>\n"),x,y);
}

#ifdef __NEVER__
FIXED floatToFixed(float x) {
  FIXED result;
  result.value = (short)floor(x);
  result.fract = (unsigned short)((x - result.value)*0x10000);
  return result;
}

MAT2 rotation(float degree) {
  MAT2 result;
  float sn = (float)sin(GRAD2RAD(degree));
  float cs = (float)cos(GRAD2RAD(degree));
  result.eM11 = floatToFixed(cs);
  result.eM12 = floatToFixed(-sn);
  result.eM21 = floatToFixed(sn);
  result.eM22 = result.eM11;
  return result;
}

MAT2 getIdentity(float size) {
  MAT2 result;
  result.eM11 = floatToFixed(size);
  result.eM12 = floatToFixed(0);
  result.eM21 = result.eM12;
  result.eM22 = result.eM11;
  return result;
}

Point2D::Point2D(const POINTFX &p) {
  x = fixedToFloat(p.x);
  y = fixedToFloat(p.y);
}
#endif
