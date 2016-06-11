#pragma once

#include <NumberInterval.h>
#include <Math/Point2D.h>
#include <Math/Rectangle2D.h>

class DataRange {
private:
  DoubleInterval m_xInterval, m_yInterval;
public:
  void init(const Point2D &p);
  void update(const Point2D &p);
  void update(const DataRange &dataRange);
  
  DataRange();
  DataRange(const Rectangle2D &r);
  DataRange(const DoubleInterval &xInterval, const DoubleInterval &yInterval);
  DataRange(double minX, double maxX, double minY, double maxY);

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
  operator Rectangle2D() const;
};

