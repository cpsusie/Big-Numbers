#pragma once

#include <NumberInterval.h>
#include "NumberPoint2D.h"

template <class T> class NumberRectangle {
public:
  T x, y, w, h;
  NumberRectangle() {
    x = y = w = h = 0;
  }

  NumberRectangle(const CRect &r) {
    x = r.left;
    y = r.top;
    w = r.right  - r.left;
    h = r.bottom - r.top;
  }

  inline NumberRectangle(const T &_x, const T &_y, const T &_w, const T &_h) : x(_x), y(_y), w(_w), h(_h) {
  }
#ifdef LONGDOUBLE
  NumberRectangle(double _x, double _y, double _w, double _h) : x(_x), y(_y), w(_w), h(_h) {
  }
#endif
  inline const T &getX() const {
    return x;
  }

  inline const T &getY() const {
    return y;
  }

  inline const T &getWidth() const {
    return w;
  }

  inline const T &getHeight() const {
    return h;
  }

  inline NumberPoint2D<T> getTopLeft()     const {
    return NumberPoint2D<T>(x,y);
  }
  inline NumberPoint2D<T> getTopRight()    const {
    return NumberPoint2D<T>(x+w,y);
  }
  inline NumberPoint2D<T> getBottomLeft()  const {
    return NumberPoint2D<T>(x,y+h);
  }
  inline NumberPoint2D<T> getBottomRight() const {
    return NumberPoint2D<T>(x+w,y+h);
  }
  inline NumberPoint2D<T> getCenter()      const {
    return NumberPoint2D<T>(x+w/2,y+h/2);
  }

  inline T getArea() const {
    return w*h;
  }

  inline T getMinX() const {
    return min(x, x+w);
  }

  inline T getMaxX() const {
    return max(x, x+w);
  }

  inline T getMinY() const {
    return min(y, y+h);
  }

  inline T getMaxY() const {
    return max(y, y+h);
  }

  inline NumberInterval<T> getXInterval() const {
    return NumberInterval<T>(getMinX(), getMaxX());
  }

  inline NumberInterval<T> getYInterval() const {
    return NumberInterval<T>(getMinY(), getMaxY());
  }

  inline bool contains(const NumberPoint2D<T> &p) const {
    return (getMinX() <= p.x) && (p.x <= getMaxX()) && (getMinY() <= p.y) && (p.y <= getMaxY());
  }

  inline bool contains(const NumberRectangle<T> &rect) const {
    return contains(rect.getTopLeft()) && contains(rect.getBottomRight());
  }

  inline operator CRect() const {
    return CRect(getInt(x),getInt(y),getInt(x+w),getInt(y+h));
  }
};
/*
inline int getArea(const CSize &sz) {
  return sz.cx * sz.cy;
}

inline int getArea(const CRect &r) {
  return getArea(r.Size());
}
*/
