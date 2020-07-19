#pragma once

#include <Math/Cube3D.h>
#include <MFCUtil/PolygonCurveBase.h>

template<typename T> class PolygonCurve3DTemplate : public _PolygonCurveTypeName {
public:
  short                   m_type; // TT_PRIM_LINE, TT_PRIM_QSPLINE or TT_PRIM_CSPLINE
  Point3DTemplateArray<T> m_points;
  template<typename P> void addPoint(const Point3DTemplate<P> &p) {
    m_points.add(p);
  }

  PolygonCurve3DTemplate(short type=0) {
    m_type = type;
  }
  template<typename P> PolygonCurve3DTemplate(const PolygonCurve3DTemplate<P> &src)
    : m_type(src.m_type)
    , m_points(src.m_points)
  {
  }

  Cube3DTemplate<T> getBoundingBox() const {
    return m_points.getBoundingBox();
  }
  inline const Point3DTemplateArray<T> &getAllPoints() const {
    return m_points;
  }
  template<typename P> void move(const Point3DTemplate<P> &dp) {
    for(size_t i = 0; i < m_points.size(); i++) {
      m_points[i] += dp;
    }
  }
  inline short getType() const {
    return m_type;
  }
  String toString() const {
    return format(_T("%s\n"), typeToStr(m_type)) + indentString(m_points.toString(_T("\n")),4);
  }
};

class PolygonCurve3D : public PolygonCurve3DTemplate<double> {
public:
  PolygonCurve3D(short type=0) : PolygonCurve3DTemplate(type) {
  }
  template<typename P> PolygonCurve3D(const PolygonCurve3DTemplate<P> &src) : PolygonCurve3DTemplate<double>(src) {
  }
};

class ProfileCurve3D {
public:
  // TT_PRIM_LINE, TT_PRIM_QSPLINE or TT_PRIM_CSPLINE
  short          m_type;
  Point3DArray   m_points;

  inline ProfileCurve3D(short type=0) {
    m_type = type;
  }

  ProfileCurve3D(const PolygonCurve3D &src);
  operator PolygonCurve3D() const;

  inline ProfileCurve3D &addPoint(const Point3D &p) {
    m_points.add(p); return *this;
  }
  inline ProfileCurve3D &addPoint(double x, double y, double z) {
    return addPoint(Point3D(x,y,z));
  }
  inline Point3D &getLastPoint() {
    return m_points.last();
  }
  inline const Point3D &getLastPoint() const {
    return m_points.last();
  }
  inline bool isEmpty() const {
    return m_points.isEmpty();
  }
  // return true if empty after removing last point
  inline bool removeLastPoint() {
    if(isEmpty()) return true;
    m_points.removeLast();
    return m_points.isEmpty();
  }
  Cube3D                 getBoundingBox() const;
  const Point3DArray    &getAllPoints() const;
  CompactArray<Point3D*> getAllPointsRef();
  void move(const Point3D &dp);
  String toString() const;
};

inline bool operator==(const ProfileCurve3D   &p1, const ProfileCurve3D   &p2) {
  return (p1.m_type == p2.m_type) && (p1.m_points == p2.m_points);
}

inline bool operator!=(const ProfileCurve3D   &p1, const ProfileCurve3D   &p2) {
  return !(p1==p2);
}

class ProfilePolygon3D {
public:
  Point3D               m_start;
  Array<ProfileCurve3D> m_curveArray;
  bool                  m_closed;

  ProfilePolygon3D();

  inline void addCurve(const ProfileCurve3D &curve) {
    m_curveArray.add(curve);
  }

  Cube3D                 getBoundingBox() const;
  Point3DArray           getAllPoints() const;
  inline ProfileCurve3D &getLastCurve() {
    return m_curveArray.last();
  }

  inline const ProfileCurve3D &getLastCurve() const {
    return m_curveArray.last();
  }

  inline Point3D &getLastPoint() {
    return getLastCurve().getLastPoint();
  }

  inline const Point3D &getLastPoint() const {
    return getLastCurve().getLastPoint();
  }

  inline void removeLastPoint() {
    getLastCurve().removeLastPoint();
  }
  bool isEmpty() const;

  String toString() const;
};

inline bool operator==(const ProfilePolygon3D &p1, const ProfilePolygon3D &p2) {
  return p1.m_start      == p2.m_start
      && p1.m_closed     == p2.m_closed
      && p1.m_curveArray == p2.m_curveArray;
}

inline bool operator!=(const ProfilePolygon3D &p1, const ProfilePolygon3D &p2) {
  return !(p1==p2);
}

class Profile3D {
public:
  String                  m_name;
  Array<ProfilePolygon3D> m_polygonArray;
  void init();
  Profile3D() {
    init();
  }
  bool hasDefaultName() const;
  String getDisplayName() const;

  void addLine(const Point3D &p1, const Point3D &p2);
  // n is number of points. number of lines will be n-1
  void addLineStrip(const Point3D *points, int n);

  inline void addPolygon(const ProfilePolygon3D &polygon) {
    m_polygonArray.add(polygon);
  }
  ProfilePolygon3D &getLastPolygon() {
    return m_polygonArray.last();
  }
  bool            isEmpty() const;
  Cube3D          getBoundingBox() const;
  Point3DArray    getAllPoints() const;
};

inline bool operator==(const Profile3D &p1, const Profile3D &p2) {
  return (p1.m_name == p2.m_name) && (p1.m_polygonArray == p2.m_polygonArray);
}

inline bool operator!=(const Profile3D &p1, const Profile3D &p2) {
  return !(p1==p2);
}
