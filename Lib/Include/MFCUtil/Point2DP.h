#pragma once

#include <Math/Rectangle2D.h>

inline float fixedToFloat(const FIXED &x) {
  return (float)x.value + (float)x.fract / 0x10000u;
}
FIXED floatToFixed(float x);
MAT2  getMAT2Rotation(float degree);
MAT2  getMAT2Id(float size = 1.0f);


class Point2DP : public Point2D {
public:
  inline Point2DP() {
  }
  inline Point2DP(double _x, double _y) : Point2D(_x, _y) {
  }
  inline Point2DP(const Point2D &p) : Point2D(p) {
  }
  inline Point2DP(const CPoint &p) : Point2D(p.x, p.y) {
  }
  inline Point2DP(const POINTFX &p) : Point2D(fixedToFloat(p.x), fixedToFloat(p.y)) {
  }

  inline operator CPoint() const {
    return CPoint((int)round(x), (int)round(y));
  }
};

class Rectangle2DR : public Rectangle2D {
public:
  inline Rectangle2DR() {
  }
  inline Rectangle2DR(double x, double y, double w, double h) : Rectangle2D(x,y,w,h) {
  }
  inline Rectangle2DR(const Rectangle2D &r) : Rectangle2D(r) {
  }
  inline Rectangle2DR(const CRect &r) : Rectangle2D(r.left, r.top, r.right - r.left, r.bottom - r.top) {
  }

  inline operator CRect() const {
    return CRect((int)m_x,(int)m_y,(int)(m_x+m_w),(int)(m_y+m_h));
  }
  inline Point2DP getProjection(const Point2DP &p) const {
    return Rectangle2D::getProjection(p);
  }
  inline static Rectangle2DR makeBottomUpRectangle(const Rectangle2DR &rect) {
    return Rectangle2D::makeBottomUpRectangle(rect);
  }
  inline static Rectangle2DR makePositiveRectangle(const Rectangle2DR &rect) {
    return Rectangle2D::makePositiveRectangle(rect);
  }
};

