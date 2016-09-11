#pragma once

#include <MyUtil.h>
#include <NumberInterval.h>
#include "Point2D.h"

class Rectangle2D {
public:
  double m_x, m_y, m_w, m_h;
  inline Rectangle2D() : m_x(0), m_y(0), m_w(0), m_h(0) {
  }

  inline Rectangle2D(double x, double y, double w, double h) : m_x(x), m_y(y), m_w(w), m_h(h) {
  }
  
  inline const double &getX() const {
    return m_x;
  }
  
  inline const double &getY() const {
    return m_y;
  }
  
  inline const double &getWidth() const {
    return m_w;
  }
  
  inline const double &getHeight() const {
    return m_h;
  }

  inline Point2D getSize() const {
    return Point2D(m_w, m_h);
  }

  inline Point2D getTopLeft() const {
    return Point2D(m_x,m_y);
  }

  inline Point2D getTopRight() const {
    return Point2D(m_x + m_w, m_y);
  }

  inline Point2D getBottomLeft()  const {
    return Point2D(m_x, m_y + m_h);
  }

  inline Point2D getBottomRight() const {
    return Point2D(m_x + m_w, m_y + m_h);
  }

  inline Point2D getCenter() const {
    return Point2D(m_x + m_w/2, m_y + m_h/2);
  }

  inline double getArea() const {
    return m_w * m_h;
  }

  inline double getMinX() const {
    return min(m_x, m_x + m_w);
  }

  inline double getMaxX() const {
    return max(m_x, m_x + m_w);
  }

  inline double getMinY() const {
    return min(m_y, m_y + m_h);
  }
  
  inline double getMaxY() const {
    return max(m_y, m_y + m_h);
  }

  inline DoubleInterval getXInterval() const {
    return DoubleInterval(getMinX(), getMaxX());
  }

  inline DoubleInterval getYInterval() const {
    return DoubleInterval(getMinY(), getMaxY());
  }

  inline bool contains(const Point2D &p) const {
    return (getMinX() <= p.x) && (p.x <= getMaxX()) && (getMinY() <= p.y) && (p.y <= getMaxY());
  }

  inline Rectangle2D &operator+=(const Point2D &dp) {
    m_x += dp.x; m_y += dp.y;
    return *this;
  }
  inline Rectangle2D &operator-=(const Point2D &dp) {
    m_x -= dp.x; m_y -= dp.y;
    return *this;
  }
  inline bool contains(const Rectangle2D &rect) const {
    return contains(rect.getTopLeft()) && contains(rect.getBottomRight());
  }

  inline Point2D getProjection(const Point2D &p) const {
    return Point2D(minMax(p.x, m_x, m_x + m_w), minMax(p.y, m_y, m_y + m_h));
  }

  // Returns a Rectangle2D with non-negative width and non-positive height
  inline static Rectangle2D makeBottomUpRectangle(const Rectangle2D &rect) {
    return Rectangle2D(rect.getMinX(),rect.getMaxY(), rect.getMaxX() - rect.getMinX(), rect.getMinY() - rect.getMaxY());
  }

  // Returns a Rectangle2D with non-negative width and height
  inline static Rectangle2D makePositiveRectangle(const Rectangle2D &rect) {
    return Rectangle2D(rect.getMinX(),rect.getMinY(), rect.getMaxX() - rect.getMinX(), rect.getMaxY() - rect.getMinY());
  }
};

Rectangle2D getUnion(const Rectangle2D &r1, const Rectangle2D &r2);

class Point2DArray : public CompactArray<Point2D> {
public:
  Point2DArray() {
  }
  explicit Point2DArray(size_t capacity) : CompactArray<Point2D>(capacity) {
  }
  Rectangle2D getBoundingBox() const;
};

class Point2DRefArray : public CompactArray<Point2D*> {
public:
  String toString(int dec = 3) const;
};

