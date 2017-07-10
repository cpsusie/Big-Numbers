#pragma once

#include "MyUtil.h"         // MFC core and standard components
#include <Math.h>
#include <Math/MathLib.h>

class Size2D {
public:
  double cx, cy;
  inline Size2D() {
    cx = cy = 0;
  }

  inline Size2D(double _cx, double _cy) : cx(_cx), cy(_cy) {
  }
  inline Size2D operator-(const Size2D &s) const {
    return Size2D(cx-s.cx, cy-s.cy);
  }
  inline Size2D operator+(const Size2D &s) const {
    return Size2D(cx+s.cx, cy+s.cy);
  }
  friend inline Size2D operator*(double factor, const Size2D &s) {
    return Size2D(factor*s.cx, factor*s.cy);
  }
  inline Size2D operator*(double factor) const {
    return Size2D(cx*factor, cy*factor);
  }
  inline Size2D operator/(double factor) const {
    return Size2D(cx/factor, cy/factor);
  }
  inline Size2D &operator*=(double factor) {
    cx *= factor;
    cy *= factor;
    return *this;
  }
  inline Size2D &operator/=(double factor) {
    cx /= factor;
    cy /= factor;
    return *this;
  }

  inline double length() const {
    return sqrt(cx*cx + cy*cy);
  }
  inline double area() const {
    return cx*cy;
  }
  inline Size2D &normalize() {
    const double l = length();
    if(l != 0) {
      *this /= l;
    }
    return *this;
  }

  inline bool operator==(const Size2D &s) const {
    return (cx==s.cx) && (cy==s.cy);
  }
  inline bool operator!=(const Size2D &s) const {
    return !(*this == s);
  }
  inline String toString(int dec = 3) const {
    const int n = dec+3;
    return format(_T("(% *.*f,% *.*f)"), n,dec,cx, n,dec,cy);
  }
};

class Point2D {
public:
  double x, y;
  inline Point2D() {
    x = y = 0;
  }

  inline Point2D(double _x, double _y) : x(_x), y(_y) {
  }
  explicit Point2D(const String &s);
  inline Point2D(const Size2D &s) : x(s.cx), y(s.cy) {
  }
//  Point2D(const POINTFX &p);

  inline Point2D operator-() const {
    return Point2D(-x, -y);
  }
  inline Point2D operator+(const Point2D &p) const {
    return Point2D(x+p.x, y+p.y);
  }
  inline Point2D operator-(const Point2D &p) const {
    return Point2D(x-p.x, y-p.y);
  }
  inline Point2D operator+(const Size2D &s) const {
    return Point2D(x+s.cx, y+s.cy);
  }
  inline Point2D operator-(const Size2D &s) const {
    return Point2D(x-s.cx, y-s.cy);
  }

  inline Point2D operator%(const Point2D &p) const { // x+=p1.x, y-=p1.y
    return Point2D(x+p.x, y-p.y);
  }
  inline Point2D operator*(double factor) const {
    return Point2D(x*factor, y*factor);
  }
  inline Point2D operator/(double factor) const {
    return Point2D(x/factor, y/factor);
  }

  inline double operator*(const Point2D &p) const {
    return x*p.x + y*p.y;
  }
  inline Point2D &operator+=(const Point2D &p) {
    x += p.x;
    y += p.y;
    return *this;
  }
  inline Point2D &operator-=(const Point2D &p) {
    x -= p.x;
    y -= p.y;
    return *this;
  }
  inline Point2D &operator+=(const Size2D &s) {
    x += s.cx;
    y += s.cy;
    return *this;
  }
  inline Point2D &operator-=(const Size2D &s) {
    x -= s.cx;
    y -= s.cy;
    return *this;
  }
  inline Point2D &operator*=(double factor) {
    x *= factor;
    y *= factor;
    return *this;
  }
  inline Point2D &operator/=(double factor) {
    x /= factor;
    y /= factor;
    return *this;
  }
  Point2D rotate(double rad) const;
  inline double length() const {
    return sqrt(x*x + y*y);
  }

  inline Point2D &normalize() {
    const double l = length();
    if(l != 0) {
      x/=l; y/=l;
    }
    return *this;
  }

  inline bool operator==(const Point2D &p) const {
    return (x==p.x) && (y==p.y);
  }
  inline bool operator!=(const Point2D &p) const {
    return (x != p.x) || (y != p.y);
  }
  inline bool operator<(const Point2D &p) const {
    return (x < p.x) && (y < p.y);
  }
  inline bool operator<=(const Point2D &p) const {
    return (x <= p.x) && (y <= p.y);
  }
  inline String toString(int dec = 3) const {
    const int n = dec+3;
    return format(_T("(% *.*f,% *.*f)"), n,dec,x, n,dec,y);
  }

  String toXML() const;
};

class Line2D {
public:
  Point2D m_p1, m_p2;
  Line2D(const Point2D &p1, const Point2D &p2) { m_p1 = p1; m_p2 = p2; }
};

inline double distance(const Point2D &p1, const Point2D &p2) {
  return sqrt(sqr(p1.x - p2.x) + sqr(p1.y - p2.y));
}

Point2D pointOfIntersection(    const Line2D  &line1, const Line2D  &line2, bool &intersect);
double  distanceFromLine(       const Line2D  &line , const Point2D &p);
double  distanceFromLineSegment(const Line2D  &line , const Point2D &p);
double  distanceFromLine(       const Point2D &lp0  , const Point2D &lp1, const Point2D &p);
double  distanceFromLineSegment(const Point2D &lp0  , const Point2D &lp1, const Point2D &p);

inline Point2D unit(const Point2D &p) {
  return Point2D(p).normalize();
}

inline Point2D operator*(double factor, const Point2D &p) {
  return Point2D(p.x*factor, p.y*factor);
}

inline double det(const Point2D &p1, const Point2D &p2) {
  return p1.x*p2.y - p1.y*p2.x;
}

inline bool isParallel(const Line2D &line1, const Line2D &line2) {
  return det(line1.m_p2 - line1.m_p1, line2.m_p2 - line2.m_p1) == 0;
}

double angle(const Point2D &p1, const Point2D &p2);  // angle in radians between p1 and p2

class Point2DOperator {
public:
  virtual void apply(const Point2D &p) = 0;
};

typedef FunctionTemplate<Point2D, Point2D> Point2DFunction;
typedef FunctionTemplate<Point2D, Real>    Function2D;


//inline float fixedToFloat(const FIXED &x) {
//  return (float)x.value + (float)x.fract / 0x10000u;
//}

//FIXED floatToFixed(float x);
//MAT2 rotation(float degree);
//MAT2 getIdentity(float size = 1.0f);

