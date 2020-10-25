#include "pch.h"
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

DataRange::DataRange(const Point2D &p) {
  init(p[0], p[0], p[1], p[1]);
}

DataRange::DataRange(const Point2DArray &pa) {
  if(pa.size() > 0) {
    const Point2D *p = (Point2D*)pa.begin(), *end = (Point2D*)pa.end();
    init(p->x(), p->x(), p->y(), p->y());
    p++;
    while(p < end) {
      *this += *(p++);
    }
  }
}

DataRange &DataRange::operator+=(const Point2DArray &pa) {
  if(pa.size() > 0) {
    for(const Point2D p : pa) {
      *this += p;
    }
  }
  return *this;
}

DataRange &DataRange::operator+=(const Point2D &p) {
  if(p.x() < m_xInterval.getFrom()) {
    m_xInterval.setFrom(p.x());
  } else if(p.x() > m_xInterval.getTo()) {
    m_xInterval.setTo(p.x());
  }

  if(p.y() < m_yInterval.getFrom()) {
    m_yInterval.setFrom(p.y());
  } else if(p.y() > m_yInterval.getTo()) {
    m_yInterval.setTo(p.y());
  }
  return *this;
}

DataRange &DataRange::operator+=(const DataRange &dr) {
  m_xInterval += dr.getXInterval();
  m_yInterval += dr.getYInterval();
  return *this;
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
