#pragma once

#include "Point3D.h"

class Cube3D {
public:
  Point3D m_lbn;
  Point3D m_rtf;
  Cube3D() {
  }
  Cube3D(const Point3D &lbn, const Point3D &rtf) : m_lbn(lbn), m_rtf(rtf) {
  }
  inline double getLength() const { // dimension x
    return m_rtf.x - m_lbn.x;
  }
  inline double getHeight() const { // dimension y
    return m_rtf.y - m_lbn.y;
  }
  inline double getDepth() const {  // dimension z
    return m_rtf.z - m_lbn.z;
  }
  inline double volume() const {
    return getLength() * getHeight() * getDepth();
  }
  inline double getMinX() const {
    return m_lbn.x;
  }
  inline double getMaxX() const {
    return m_rtf.x;
  }
  inline double getMinY() const {
    return m_lbn.y;
  }
  inline double getMaxY() const {
    return m_rtf.y;
  }
  inline double getMinZ() const {
    return m_lbn.z;
  }
  inline double getMaxZ() const {
    return m_rtf.z;
  }
  inline bool contains(const Point3D &p) const {
    return (m_lbn <= p) && (p <= m_rtf);
  }
  inline bool contains(const Cube3D &cube) const {
    return (m_lbn <= cube.m_lbn) && (cube.m_rtf <= m_rtf);
  }
};
