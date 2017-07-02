#pragma once

#include <NumberInterval.h>
#include <Math/Point2D.h>
#include <Math/Rectangle2D.h>

class DataRange {
private:
  DoubleInterval m_xInterval, m_yInterval;
public:
  DataRange();
  DataRange(const Point2D      &p );
  DataRange(const Rectangle2D  &r );
  DataRange(const Point2DArray &pa);
  DataRange(const DoubleInterval &xInterval, const DoubleInterval &yInterval);
  DataRange(double minX, double maxX, double minY, double maxY);

  DataRange &operator+=(const Point2D      &p ); // adjust intervals to include p
  DataRange &operator+=(const DataRange    &dr); // union of *this  and dr
  DataRange &operator+=(const Point2DArray &pa);

  double getMinX() const {
    return m_xInterval.getFrom();
  }

  double getMaxX() const {
    return m_xInterval.getTo();
  }

  double getMinY() const {
    return m_yInterval.getFrom();
  }

  double getMaxY() const {
    return m_yInterval.getTo();
  }

  const DoubleInterval &getXInterval() const {
    return m_xInterval;
  }

  const DoubleInterval &getYInterval() const {
    return m_yInterval;
  }

  void init(double minX, double maxX, double minY, double maxY);
  inline void setToDefault() { // used for empty pointArray in PointGraph
    init(-1,1,-1,1);
  }
  operator Rectangle2D() const;
};

