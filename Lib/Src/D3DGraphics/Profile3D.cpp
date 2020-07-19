#include "pch.h"
#include <FileNameSplitter.h>
#include <D3DGraphics/Profile3D.h>

// ------------------------------------ ProfileCurve3D ------------------------------

ProfileCurve3D::ProfileCurve3D(const PolygonCurve3D &src) {
  m_type   = src.getType();
  m_points = src.getAllPoints();
}

ProfileCurve3D::operator PolygonCurve3D() const {
  PolygonCurve3D result;
  result.m_type   = m_type;
  result.m_points = m_points;
  return result;
}

void ProfileCurve3D::move(const Point3D &dp) {
  for(size_t i = 0; i < m_points.size(); i++) {
    m_points[i] += dp;
  }
}

const Point3DArray &ProfileCurve3D::getAllPoints() const {
  return m_points;
}

CompactArray<Point3D*> ProfileCurve3D::getAllPointsRef() {
  CompactArray<Point3D*> result(m_points.size());
  for(size_t i = 0; i < m_points.size(); i++) {
    result.add(&m_points[i]);
  }
  return result;
}

Cube3D ProfileCurve3D::getBoundingBox() const {
  return m_points.getBoundingBox();
}

String ProfileCurve3D::toString() const {
  return ((PolygonCurve3D*)(this))->toString();
}

// ------------------------------------ ProfilePolygon3D ------------------------------

ProfilePolygon3D::ProfilePolygon3D() {
  m_closed = false;
}

Point3DArray ProfilePolygon3D::getAllPoints() const {
  Point3DArray result;
  result.add(m_start);
  for(size_t i = 0; i < m_curveArray.size(); i++) {
    result.addAll(m_curveArray[i].getAllPoints());
  }
  return result;
}

Cube3D ProfilePolygon3D::getBoundingBox() const {
  return getAllPoints().getBoundingBox();
}

bool ProfilePolygon3D::isEmpty() const {
  for(size_t i = 0; i < m_curveArray.size(); i++) {
    if(!m_curveArray[i].isEmpty()) {
      return false;
    }
  }
  return true;
}

String ProfilePolygon3D::toString() const {
  String result = format(_T("start:%s\n"), m_start.toString().cstr());
  for(size_t p = 0; p < m_curveArray.size(); p++) {
    result += m_curveArray[p].toString() + _T("\n");
  }
  return result;
}

// ------------------------------------ Profile3D ------------------------------

Point3DArray Profile3D::getAllPoints() const {
  Point3DArray result;
  for(size_t i = 0; i < m_polygonArray.size(); i++) {
    result.addAll(m_polygonArray[i].getAllPoints());
  }
  return result;
}

bool Profile3D::isEmpty() const {
  for(size_t i = 0; i < m_polygonArray.size(); i++) {
    if(!m_polygonArray[i].isEmpty()) {
      return false;
    }
  }
  return true;
}

Cube3D Profile3D::getBoundingBox() const {
  return getAllPoints().getBoundingBox();
}

void Profile3D::addLine(const Point3D &p1, const Point3D &p2) {
  ProfilePolygon3D polygon;
  ProfileCurve3D   curve(TT_PRIM_LINE);
  polygon.m_start  = p1;
  polygon.m_closed = false;
  curve.m_points.add(p2);
  polygon.addCurve(curve);
  addPolygon(polygon);
}

void Profile3D::addLineStrip(const Point3D *points, int n) {
  const Point3D *last = points++;
  while(--n) {
    addLine(*last, *points);
    last = points++;
  }
}

void Profile3D::init() {
  m_name = _T("Untitled");
  m_polygonArray.clear();
}

bool Profile3D::hasDefaultName() const {
  return m_name != EMPTYSTRING && m_name != _T("Untitled");
}

String Profile3D::getDisplayName() const {
  return FileNameSplitter(m_name).getFileName();
}
