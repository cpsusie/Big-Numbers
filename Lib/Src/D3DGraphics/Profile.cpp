#include "pch.h"
#include <FileNameSplitter.h>
#include <MFCUtil/PolygonCurve.h>
#include <MFCUtil/ShapeFunctions.h>
#include <D3DGraphics/Profile.h>

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

// ------------------------------------ ProfileCurve ------------------------------

ProfileCurve::ProfileCurve(const PolygonCurve &src) {
  m_type   = src.getType();
  m_points = src.getAllPoints();
}

ProfileCurve::operator PolygonCurve() const {
  PolygonCurve result;
  result.m_type   = m_type;
  result.m_points = m_points;
  return result;
}

void ProfileCurve::move(const Point2D &dp) {
  for(size_t i = 0; i < m_points.size(); i++) {
    m_points[i] += dp;
  }
}

const Point2DArray &ProfileCurve::getAllPoints() const {
  return m_points;
}

CompactArray<Point2D*> ProfileCurve::getAllPointsRef() {
  CompactArray<Point2D*> result(m_points.size());
  for(size_t i = 0; i < m_points.size(); i++) {
    result.add(&m_points[i]);
  }
  return result;
}

Rectangle2D ProfileCurve::getBoundingBox() const {
  return m_points.getBoundingBox();
}

String ProfileCurve::toString() const {
  return ((PolygonCurve*)(this))->toString();
}

bool operator==(const ProfileCurve   &p1, const ProfileCurve   &p2) {
  return (p1.m_type == p2.m_type) && (p1.m_points == p2.m_points);
}

bool operator!=(const ProfileCurve   &p1, const ProfileCurve   &p2) {
  return !(p1==p2);
}

// ------------------------------------ ProfilePolygon ------------------------------

ProfilePolygon::ProfilePolygon() {
  m_closed = false;
}
/*
ProfilePolygon::ProfilePolygon(const GlyphPolygon &src) {
  m_start = src.m_start;
  m_closed = true;
  for(size_t i = 0; i < src.m_polygonCurveArray.size(); i++) {
    addCurve(src.m_polygonCurveArray[i]);
  }
}
*/

Point2DArray ProfilePolygon::getAllPoints() const {
  Point2DArray result;
  result.add(m_start);
  for(size_t i = 0; i < m_curveArray.size(); i++) {
    result.addAll(m_curveArray[i].getAllPoints());
  }
  return result;
}

CompactArray<Point2D*> ProfilePolygon::getAllPointsRef() {
  CompactArray<Point2D*> result;
  result.add(&m_start);
  for(size_t i = 0; i < m_curveArray.size(); i++) {
    result.addAll(m_curveArray[i].getAllPointsRef());
  }
  return result;
}

Point2DArray ProfilePolygon::getCurvePoints() const {
  PointCollector collector;
  apply(collector);
  if(m_closed) {
    collector.m_result.removeLast();
  }
  return collector.m_result;
}

Vertex2DArray ProfilePolygon::getFlatVertexArray() const {
  FlatVertexGenerator vg;
  apply(vg);
  return vg.getResult();
}

Vertex2DArray ProfilePolygon::getSmoothVertexArray() const { // return noOfPoints normals
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

Rectangle2D ProfilePolygon::getBoundingBox() const {
  return getAllPoints().getBoundingBox();
}

bool ProfilePolygon::isEmpty() const {
  for(size_t i = 0; i < m_curveArray.size(); i++) {
    if(!m_curveArray[i].isEmpty()) {
      return false;
    }
  }
  return true;
}

void ProfilePolygon::move(const Point2D &dp) {
  m_start += dp;
  for(size_t i = 0; i < m_curveArray.size(); i++) {
    m_curveArray[i].move(dp);
  }
}

void ProfilePolygon::reverseOrder() {
  ProfilePolygon result;
  result.m_closed = m_closed;
  Point2DArray points = getAllPoints();
  int pindex = (int)points.size() - 1;
  result.m_start = points[pindex--];
  for(intptr_t i = m_curveArray.size()-1; i >= 0; i--) {
    ProfileCurve &curve = m_curveArray[i];
    ProfileCurve newCurve(curve.m_type);
    for(size_t j = 0; j < curve.m_points.size(); j++) {
      newCurve.addPoint(points[pindex--]);
    }
    result.addCurve(newCurve);
  }

//  String t1 = toString();
//  String t2 = result.toString();
  *this = result;
}

String ProfilePolygon::toString() const {
  String result = format(_T("start:%s\n"), m_start.toString().cstr());
  for(size_t p = 0; p < m_curveArray.size(); p++) {
    result += m_curveArray[p].toString() + _T("\n");
  }
  return result;
}

bool operator==(const ProfilePolygon &p1, const ProfilePolygon &p2) {
  return p1.m_start == p2.m_start
      && p1.m_closed == p2.m_closed
      && p1.m_curveArray == p2.m_curveArray;
}

bool operator!=(const ProfilePolygon &p1, const ProfilePolygon &p2) {
  return !(p1==p2);
}
// ------------------------------------ Profile ------------------------------

Point2DArray Profile::getAllPoints() const {
  Point2DArray result;
  for(size_t i = 0; i < m_polygonArray.size(); i++) {
    result.addAll(m_polygonArray[i].getAllPoints());
  }
  return result;
}

CompactArray<Point2D*> Profile::getAllPointsRef() {
  CompactArray<Point2D*> result;
  for(size_t i = 0; i < m_polygonArray.size(); i++) {
    result.addAll(m_polygonArray[i].getAllPointsRef());
  }
  return result;
}

Point2DArray Profile::getCurvePoints() const {
  Point2DArray result;
  for(size_t i = 0; i < m_polygonArray.size(); i++) {
    result.addAll(m_polygonArray[i].getCurvePoints());
  }
  return result;
}

bool Profile::isEmpty() const {
  for(size_t i = 0; i < m_polygonArray.size(); i++) {
    if(!m_polygonArray[i].isEmpty()) {
      return false;
    }
  }
  return true;
}

Rectangle2D Profile::getBoundingBox() const {
  return getAllPoints().getBoundingBox();
}

void Profile::addLine(const Point2D &p1, const Point2D &p2) {
  ProfilePolygon polygon;
  ProfileCurve curve(TT_PRIM_LINE);
  polygon.m_start  = p1;
  polygon.m_closed = false;
  curve.m_points.add(p2);
  polygon.addCurve(curve);
  addPolygon(polygon);
}

void Profile::addLineStrip(const Point2D *points, int n) {
  const Point2D *last = points++;
  while(--n) {
    addLine(*last, *points);
    last = points++;
  }
}

void Profile::move(const Point2D &dp) {
  for(size_t i = 0; i < m_polygonArray.size(); i++) {
    m_polygonArray[i].move(dp);
  }
}

int Profile::findPolygonContainingPoint(const Point2D *p) const {
  for(size_t i = 0; i < m_polygonArray.size(); i++) {
    const ProfilePolygon &pp = m_polygonArray[i];
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

bool Profile::canConnect(const Point2D *p1, const Point2D *p2) const {
  if(p1 == p2) {
    return false;
  }
  int pp1 = findPolygonContainingPoint(p1);
  int pp2 = findPolygonContainingPoint(p2);
  return pp1 >= 0 && pp2 >= 0 && pp1 != pp2;
}

void Profile::connect(const Point2D *p1, const Point2D *p2) {
  if(!canConnect(p1,p2)) {
    return;
  }
  int i1 = findPolygonContainingPoint(p1);
  int i2 = findPolygonContainingPoint(p2);
  ProfilePolygon &pp1 = m_polygonArray[i1];
  ProfilePolygon &pp2 = m_polygonArray[i2];

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

void ProfilePolygon::apply(CurveOperator &op) const {
  Point2D pp = m_start;
  op.beginCurve();
  op.apply(pp);
  for(size_t i = 0; i < m_curveArray.size(); i++) {
    const ProfileCurve &curve = m_curveArray[i];
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

void Profile::apply(CurveOperator &op) const {
  for(size_t i = 0; i < m_polygonArray.size(); i++) {
    m_polygonArray[i].apply(op);
  }
}

/*

LPDIRECT3DRMMESHBUILDER Profile::createSkeleton(C3D &d3) {
  LPDIRECT3DRMMESHBUILDER meshBuilder = d3.createMeshBuilder(m_name);
  apply(CurveMaker(meshBuilder));
  return meshBuilder;
}
*/

String Profile::toString() const {
  String result;
  for(size_t i = 0; i < m_polygonArray.size(); i++) {
    result += m_polygonArray[i].toString();
  }
  return result;
}

/*
Profile::Profile(const String &name, const GlyphCurveData &src) {
  m_name = name;
  const Array< GlyphPolygon> &gpa = src.getPolygonArray();
  for(int i = 0; i < gpa.size(); i++) {
    addPolygon(gpa[i]);
  }
}
*/

void Profile::init() {
  m_name = _T("Untitled");
  m_polygonArray.clear();
}

bool Profile::hasDefaultName() const {
  return m_name != EMPTYSTRING && m_name != _T("Untitled");
}

String Profile::getDisplayName() const {
  return FileNameSplitter(m_name).getFileName();
}

bool operator==(const Profile &p1, const Profile &p2) {
  return (p1.m_name == p2.m_name) && (p1.m_polygonArray == p2.m_polygonArray);
}

bool operator!=(const Profile &p1, const Profile &p2) {
  return !(p1==p2);
}

Vertex2DArray Profile::getFlatVertexArray() const {
  FlatVertexGenerator normalGenerator;
  apply(normalGenerator);
  return normalGenerator.getResult();
}

Vertex2DArray Profile::getSmoothVertexArray() const {
  Vertex2DArray result;
  for(size_t i = 0; i < m_polygonArray.size(); i++) {
    const ProfilePolygon &pp = m_polygonArray[i];
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
