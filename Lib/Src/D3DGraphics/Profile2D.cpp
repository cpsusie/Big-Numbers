#include "pch.h"
#include <FileNameSplitter.h>
#include <MFCUtil/PolygonCurve2D.h>
#include <MFCUtil/ShapeFunctions.h>
#include <D3DGraphics/Profile2D.h>

static Point2D findNormal(const Point2D &from, const Point2D &to) {
  Point2D tmp = unitVector(to - from);
  return Point2D(-tmp.y(),tmp.x());
}

class FlatVertexGenerator : public CurveOperator {
private:
  VertexCurve2D m_result;
public:
  void line(const Point2D &from, const Point2D &to);
  const VertexCurve2D &getResult() const {
    return m_result;
  }
};

void FlatVertexGenerator::line(const Point2D &from, const Point2D &to) {
  const Point2D normal = findNormal(from, to);
  m_result.add(Vertex2D(from,normal));
  m_result.add(Vertex2D(to  ,normal));
}

// ------------------------------------ ProfileCurve2D ------------------------------

ProfileCurve2D::ProfileCurve2D(const PolygonCurve2D &src) {
  m_type   = src.getType();
  m_points = src.getAllPoints();
}

ProfileCurve2D::operator PolygonCurve2D() const {
  PolygonCurve2D result;
  result.m_type   = m_type;
  result.m_points = m_points;
  return result;
}

void ProfileCurve2D::move(const Point2D &dp) {
  for(size_t i = 0; i < m_points.size(); i++) {
    m_points[i] += dp;
  }
}

const Point2DArray &ProfileCurve2D::getAllPoints() const {
  return m_points;
}

Point2DRefArray    ProfileCurve2D::getAllPointsRef() {
  return Point2DRefArray(m_points);
}

Rectangle2D ProfileCurve2D::getBoundingBox() const {
  return m_points.getBoundingBox();
}

String ProfileCurve2D::toString() const {
  return ((PolygonCurve2D*)(this))->toString();
}

// ------------------------------------ ProfilePolygon2D ------------------------------

ProfilePolygon2D::ProfilePolygon2D() {
  m_closed = false;
}

Point2DArray ProfilePolygon2D::getAllPoints() const {
  Point2DArray result;
  result.add(m_start);
  for(ConstIterator<ProfileCurve2D> it = m_curveArray.getIterator(); it.hasNext();) {
    result.addAll(it.next().getAllPoints());
  }
  return result;
}

Point2DRefArray ProfilePolygon2D::getAllPointsRef() {
  Point2DRefArray result;
  result.add(&m_start);
  for(Iterator<ProfileCurve2D> it = m_curveArray.getIterator(); it.hasNext();) {
    result.addAll(it.next().getAllPointsRef());
  }
  return result;
}

Point2DArray ProfilePolygon2D::getCurvePoints() const {
  PointCollector collector;
  apply(collector);
  if(m_closed) {
    collector.m_result.removeLast();
  }
  return collector.m_result;
}

VertexCurve2D ProfilePolygon2D::getFlatVertexCurve() const {
  FlatVertexGenerator vg;
  apply(vg);
  return vg.getResult();
}

VertexCurve2D ProfilePolygon2D::getSmoothVertexCurve() const { // return noOfPoints normals
 const Point2DArray pa = getCurvePoints();
 const size_t       n  = pa.size();
 const Point2D     &p0 = pa[0], &pl = pa.last();

  VertexCurve2D result;
  if(m_closed) {
    result.add(Vertex2D(p0, unitVector(findNormal(pl,p0) + findNormal(p0,pa[1]))));
  } else {
    result.add(Vertex2D(p0, findNormal(p0,pa[1])));
  }
  for(size_t j = 1; j < n-1; j++) {
    result.add(Vertex2D(pa[j],unitVector(findNormal(pa[j-1],pa[j]) + findNormal(pa[j],pa[j+1]))));
  }
  if(n >= 2) {
    if(m_closed) {
      result.add(Vertex2D(pl, unitVector(findNormal(pa[n-2],pl) + findNormal(pl,p0))));
    } else {
      result.add(Vertex2D(pl, findNormal(pa[n-2],pl)));
    }
  }
  return result;
}

Rectangle2D ProfilePolygon2D::getBoundingBox() const {
  return getAllPoints().getBoundingBox();
}

bool ProfilePolygon2D::isEmpty() const {
  for(size_t i = 0; i < m_curveArray.size(); i++) {
    if(!m_curveArray[i].isEmpty()) {
      return false;
    }
  }
  return true;
}

void ProfilePolygon2D::move(const Point2D &dp) {
  m_start += dp;
  for(size_t i = 0; i < m_curveArray.size(); i++) {
    m_curveArray[i].move(dp);
  }
}

void ProfilePolygon2D::reverseOrder() {
  ProfilePolygon2D result;
  result.m_closed = m_closed;
  Point2DArray points = getAllPoints();
  int pindex = (int)points.size() - 1;
  result.m_start = points[pindex--];
  for(intptr_t i = m_curveArray.size()-1; i >= 0; i--) {
    ProfileCurve2D &curve = m_curveArray[i];
    ProfileCurve2D newCurve(curve.m_type);
    for(size_t j = 0; j < curve.m_points.size(); j++) {
      newCurve.addPoint(points[pindex--]);
    }
    result.addCurve(newCurve);
  }

//  String t1 = toString();
//  String t2 = result.toString();
  *this = result;
}

String ProfilePolygon2D::toString() const {
  String result = format(_T("start:%s\n"), m_start.toString().cstr());
  return result + m_curveArray.getIterator().toString(_T("\n"));
}

// ------------------------------------ Profile2D ------------------------------

Point2DArray Profile2D::getAllPoints() const {
  Point2DArray result;
  for(ConstIterator<ProfilePolygon2D> it = m_polygonArray.getIterator(); it.hasNext();) {
    result.addAll(it.next().getAllPoints());
  }
  return result;
}

Point2DRefArray Profile2D::getAllPointsRef() {
  Point2DRefArray result;
  for(Iterator<ProfilePolygon2D> it = m_polygonArray.getIterator(); it.hasNext();) {
    result.addAll(it.next().getAllPointsRef());
  }
  return result;
}

Point2DArray Profile2D::getCurvePoints() const {
  Point2DArray result;
  for(ConstIterator<ProfilePolygon2D> it = m_polygonArray.getIterator(); it.hasNext();) {
    result.addAll(it.next().getCurvePoints());
  }
  return result;
}

bool Profile2D::isEmpty() const {
  for(size_t i = 0; i < m_polygonArray.size(); i++) {
    if(!m_polygonArray[i].isEmpty()) {
      return false;
    }
  }
  return true;
}

Rectangle2D Profile2D::getBoundingBox() const {
  return getAllPoints().getBoundingBox();
}

void Profile2D::addLine(const Point2D &p1, const Point2D &p2) {
  ProfilePolygon2D polygon;
  ProfileCurve2D   curve(TT_PRIM_LINE);
  polygon.m_start  = p1;
  polygon.m_closed = false;
  curve.m_points.add(p2);
  polygon.addCurve(curve);
  addPolygon(polygon);
}

void Profile2D::addLineStrip(const Point2D *points, int n) {
  const Point2D *last = points++;
  while(--n) {
    addLine(*last, *points);
    last = points++;
  }
}

void Profile2D::move(const Point2D &dp) {
  for(size_t i = 0; i < m_polygonArray.size(); i++) {
    m_polygonArray[i].move(dp);
  }
}

int Profile2D::findPolygonContainingPoint(const Point2D *p) const {
  for(size_t i = 0; i < m_polygonArray.size(); i++) {
    const ProfilePolygon2D &pp = m_polygonArray[i];
    if(pp.m_closed) {
      continue;
    }
    if(p == &pp.m_start) {
      return (int)i;
    } else if(p == &pp.getLastPoint()) {
      return (int)i;
    }
  }
  return -1;
}

bool Profile2D::canConnect(const Point2D *p1, const Point2D *p2) const {
  if(p1 == p2) {
    return false;
  }
  const int pp1 = findPolygonContainingPoint(p1);
  const int pp2 = findPolygonContainingPoint(p2);
  return (pp1 >= 0) && (pp2 >= 0) && (pp1 != pp2);
}

void Profile2D::connect(const Point2D *p1, const Point2D *p2) {
  if(!canConnect(p1,p2)) {
    return;
  }
  const int i1 = findPolygonContainingPoint(p1);
  const int i2 = findPolygonContainingPoint(p2);
  ProfilePolygon2D &pp1 = m_polygonArray[i1];
  ProfilePolygon2D &pp2 = m_polygonArray[i2];

  if(p1 == &pp1.m_start) {
    if(p2 == &pp2.getLastPoint()) {
      pp2.m_curveArray.addAll(pp1.m_curveArray);
      m_polygonArray.removeIndex(i1);
    } else { // p2 == &pp2.m_start
      pp2.reverseOrder();
      pp2.m_curveArray.addAll(pp1.m_curveArray);
      m_polygonArray.removeIndex(i1);
    }
  } else if(p1 == &pp1.getLastPoint()) {
    if(p2 == &pp2.getLastPoint()) {
      pp2.reverseOrder();
      pp1.m_curveArray.addAll(pp2.m_curveArray);
      m_polygonArray.removeIndex(i2);
    } else { // p2 == &pp2.m_start
      pp1.m_curveArray.addAll(pp2.m_curveArray);
      m_polygonArray.removeIndex(i2);
    }
  }
}

void ProfilePolygon2D::apply(CurveOperator &op) const {
  Point2D pp = m_start;
  op.beginCurve();
  op.apply(pp);
  for(size_t i = 0; i < m_curveArray.size(); i++) {
    const ProfileCurve2D &curve = m_curveArray[i];
    switch(curve.m_type) {
    case TT_PRIM_LINE   :
      { for(size_t j = 0; j < curve.m_points.size(); j++) {
          const Point2D &np = curve.m_points[j];
          op.apply(np);
          pp = np;
        }
      }
      break;
    case TT_PRIM_QSPLINE:
      { String str = curve.toString();
        int f = 1;
      }
      break;
    case TT_PRIM_CSPLINE:
      { for(size_t j = 0; j < curve.m_points.size(); j+=3) {
          const Point2D &end = curve.m_points[j+2];
          applyToBezier(pp,curve.m_points[j],curve.m_points[j+1],end, op,false);
          pp = end;
        }
      }
      break;
    }
  }
  if(m_closed) {
    op.apply(m_start);
  }
  op.endCurve();
}

void Profile2D::apply(CurveOperator &op) const {
  for(size_t i = 0; i < m_polygonArray.size(); i++) {
    m_polygonArray[i].apply(op);
  }
}

String Profile2D::toString() const {
  String result;
  for(size_t i = 0; i < m_polygonArray.size(); i++) {
    result += m_polygonArray[i].toString();
  }
  return result;
}

void Profile2D::init() {
  setDefaultName();
  m_polygonArray.clear();
}

VertexProfile2D &VertexProfile2D::invertNormals() {
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    (*this)[i].invertNormals();
  }
  return *this;
}

VertexProfile2D Profile2D::getVertexProfile(bool smoothNormals) const {
  const size_t    n = m_polygonArray.size();
  VertexProfile2D result(n);
  for(size_t i = 0; i < n; i++) {
    result.add(m_polygonArray[i].getVertexCurve(smoothNormals));
  }
  return result;
}
