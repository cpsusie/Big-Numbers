#include "stdafx.h"
#include <MFCUtil/Coordinatesystem/DataRange.h>

DataRange::DataRange() {
  init(0,1,0,1);
}

DataRange::DataRange(const Rectangle2D &r) {
  init(r.getMinX(), r.getMaxX(), r.getMinY(), r.getMaxY());
}

DataRange::DataRange(const DoubleInterval &xInterval, const DoubleInterval &yInterval) {
  init(xInterval.getMin(), xInterval.getMax(), yInterval.getMin(), yInterval.getMax());
}

DataRange::DataRange(double minX, double maxX, double minY, double maxY) {
  init(minX, maxX, minY, maxY);
}

void DataRange::init(const Point2D &p) { 
  init(p.x, p.x, p.y, p.y);
}

void DataRange::update(const Point2D &p) {
  if(p.x < m_xInterval.getFrom()) {
    m_xInterval.setFrom(p.x);
  } else if(p.x > m_xInterval.getTo()) {
    m_xInterval.setTo(p.x);
  }

  if(p.y < m_yInterval.getFrom()) {
    m_yInterval.setFrom(p.y);
  } else if(p.y > m_yInterval.getTo()) {
    m_yInterval.setTo(p.y);
  }
}

void DataRange::update(const DataRange &dataRange) {
  update(Point2D(dataRange.getMinX(), dataRange.getMinY()));
  update(Point2D(dataRange.getMaxX(), dataRange.getMaxY()));
}

void DataRange::init(double minX, double maxX, double minY, double maxY) {
  m_xInterval.setFrom(min(minX,maxX));
  m_xInterval.setTo(  max(minX,maxX));
  m_yInterval.setFrom(min(minY,maxY));
  m_yInterval.setTo(  max(minY,maxY));
}

DataRange::operator Rectangle2D() const {
  double minX = getMinX();
  double minY = getMinY();
  return Rectangle2D(minX, minY, getMaxX() - minX, getMaxY() - minY);
}

void DataRange::init(const Point2DArray &pa) {
  if (pa.size() > 0) {
    init(pa[0]);
  }
  for(size_t i = 1; i < pa.size(); i++) {
    update(pa[i]);
  }
}

void DataRange::update(const Point2DArray &pa) {
  for(size_t i = 0; i < pa.size(); i++) {
    update(pa[i]);
  }
}
