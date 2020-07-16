#include "pch.h"
#include <FileNameSplitter.h>
#include <MFCUtil/PolygonCurve2D.h>
#include <MFCUtil/ShapeFunctions.h>
#include <D3DGraphics/Profile2D.h>

static Point2D findNormal(const Point2D &from, const Point2D &to) {
  Point2D tmp = unitVector(to - from);
  return Point2D(-tmp.y,tmp.x);
}

class FlatVertexGenerator : public CurveOperator {
private:
  Vertex2DArray m_result;
public:
  void line(const Point2D &from, const Point2D &to);
  const Vertex2DArray &getResult() const {
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

CompactArray<Point2D*> ProfileCurve2D::getAllPointsRef() {
  CompactArray<Point2D*> result(m_points.size());
  for(size_t i = 0; i < m_points.size(); i++) {
    result.add(&m_points[i]);
  }
  return result;
}

Rectangle2D ProfileCurve2D::getBoundingBox() const {
  return m_points.getBoundingBox();
}

String ProfileCurve2D::toString() const {
  return ((PolygonCurve2D*)(this))->toString();
}

bool operator==(const ProfileCurve2D   &p1, const ProfileCurve2D   &p2) {
  return (p1.m_type == p2.m_type) && (p1.m_points == p2.m_points);
}

bool operator!=(const ProfileCurve2D   &p1, const ProfileCurve2D   &p2) {
  return !(p1==p2);
}

// ------------------------------------ ProfilePolygon2D ------------------------------

ProfilePolygon2D::ProfilePolygon2D() {
  m_closed = false;
}
/*
ProfilePolygon2D::ProfilePolygon2D(const GlyphPolygon &src) {
  m_start = src.m_start;
  m_closed = true;
  for(size_t i = 0; i < src.m_polygonCurveArray.size(); i++) {
    addCurve(src.m_polygonCurveArray[i]);
  }
}
*/

Point2DArray ProfilePolygon2D::getAllPoints() const {
  Point2DArray result;
  result.add(m_start);
  for(size_t i = 0; i < m_curveArray.size(); i++) {
    result.addAll(m_curveArray[i].getAllPoints());
  }
  return result;
}

CompactArray<Point2D*> ProfilePolygon2D::getAllPointsRef() {
  CompactArray<Point2D*> result;
  result.add(&m_start);
  for(size_t i = 0; i < m_curveArray.size(); i++) {
    result.addAll(m_curveArray[i].getAllPointsRef());
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

Vertex2DArray ProfilePolygon2D::getFlatVertexArray() const {
  FlatVertexGenerator vg;
  apply(vg);
  return vg.getResult();
}

Vertex2DArray ProfilePolygon2D::getSmoothVertexArray() const { // return noOfPoints normals
  const Point2DArray pa = getCurvePoints();
  const size_t       n  = pa.size();
  const Point2D     &p0 = pa[0], &pl = pa.last();

  Vertex2DArray result;
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
  for(size_t p = 0; p < m_curveArray.size(); p++) {
    result += m_curveArray[p].toString() + _T("\n");
  }
  return result;
}

bool operator==(const ProfilePolygon2D &p1, const ProfilePolygon2D &p2) {
  return p1.m_start == p2.m_start
      && p1.m_closed == p2.m_closed
      && p1.m_curveArray == p2.m_curveArray;
}

bool operator!=(const ProfilePolygon2D &p1, const ProfilePolygon2D &p2) {
  return !(p1==p2);
}
// ------------------------------------ Profile2D ------------------------------

Point2DArray Profile2D::getAllPoints() const {
  Point2DArray result;
  for(size_t i = 0; i < m_polygonArray.size(); i++) {
    result.addAll(m_polygonArray[i].getAllPoints());
  }
  return result;
}

CompactArray<Point2D*> Profile2D::getAllPointsRef() {
  CompactArray<Point2D*> result;
  for(size_t i = 0; i < m_polygonArray.size(); i++) {
    result.addAll(m_polygonArray[i].getAllPointsRef());
  }
  return result;
}

Point2DArray Profile2D::getCurvePoints() const {
  Point2DArray result;
  for(size_t i = 0; i < m_polygonArray.size(); i++) {
    result.addAll(m_polygonArray[i].getCurvePoints());
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
  int pp1 = findPolygonContainingPoint(p1);
  int pp2 = findPolygonContainingPoint(p2);
  return pp1 >= 0 && pp2 >= 0 && pp1 != pp2;
}

void Profile2D::connect(const Point2D *p1, const Point2D *p2) {
  if(!canConnect(p1,p2)) {
    return;
  }
  int i1 = findPolygonContainingPoint(p1);
  int i2 = findPolygonContainingPoint(p2);
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

/*

LPDIRECT3DRMMESHBUILDER Profile2D::createSkeleton(C3D &d3) {
  LPDIRECT3DRMMESHBUILDER meshBuilder = d3.createMeshBuilder(m_name);
  apply(CurveMaker(meshBuilder));
  return meshBuilder;
}
*/

String Profile2D::toString() const {
  String result;
  for(size_t i = 0; i < m_polygonArray.size(); i++) {
    result += m_polygonArray[i].toString();
  }
  return result;
}

/*
Profile2D::Profile2D(const String &name, const GlyphCurveData &src) {
  m_name = name;
  const Array< GlyphPolygon> &gpa = src.getPolygonArray();
  for(int i = 0; i < gpa.size(); i++) {
    addPolygon(gpa[i]);
  }
}
*/

void Profile2D::init() {
  m_name = _T("Untitled");
  m_polygonArray.clear();
}

bool Profile2D::hasDefaultName() const {
  return m_name != EMPTYSTRING && m_name != _T("Untitled");
}

String Profile2D::getDisplayName() const {
  return FileNameSplitter(m_name).getFileName();
}

bool operator==(const Profile2D &p1, const Profile2D &p2) {
  return (p1.m_name == p2.m_name) && (p1.m_polygonArray == p2.m_polygonArray);
}

bool operator!=(const Profile2D &p1, const Profile2D &p2) {
  return !(p1==p2);
}

Vertex2DArray Profile2D::getFlatVertexArray() const {
  FlatVertexGenerator normalGenerator;
  apply(normalGenerator);
  return normalGenerator.getResult();
}

Vertex2DArray Profile2D::getSmoothVertexArray() const {
  Vertex2DArray result;
  for(size_t i = 0; i < m_polygonArray.size(); i++) {
    const ProfilePolygon2D &pp = m_polygonArray[i];
    result.addAll(pp.getSmoothVertexArray());
  }
  return result;
}

bool operator==(const ProfileRotationParameters &p1, const ProfileRotationParameters &p2) {
  return p1.m_rotateAxis         == p2.m_rotateAxis
      && p1.m_rotateAxisAlignsTo == p2.m_rotateAxisAlignsTo
      && p1.m_edgeCount          == p2.m_edgeCount
      && p1.m_rad                == p2.m_rad
      && p1.m_flags              == p2.m_flags
      && p1.m_color              == p2.m_color;
}

bool operator!=(const ProfileRotationParameters &p1, const ProfileRotationParameters &p2) {
  return !(p1==p2);
}

ProfileStretchParameters::ProfileStretchParameters() {
}
