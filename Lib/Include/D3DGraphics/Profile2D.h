#pragma once

#include <FlagTraits.h>
#include <PersistentDataTemplate.h>
#include <MFCUtil/ShapeFunctions.h>

template<typename T> class Vertex2DTemplate {
public:
  Point2DTemplate<T> m_pos;
  Point2DTemplate<T> m_normal;
  Vertex2DTemplate() {
  }
  template<typename V> Vertex2DTemplate(const Vertex2DTemplate<V> &v) : m_pos(v.pos), m_normal(v.normal) {
  }
  template<typename P, typename N> Vertex2DTemplate(const FixedSizeVectorTemplate<P,2> &pos, const FixedSizeVectorTemplate<N,2> &normal)
    : m_pos(pos), m_normal(normal)
  {
  }
  template<typename V> Vertex2DTemplate<T> &operator=(const Vertex2DTemplate<V> &v) {
    return setPos(v.pos).setNormal(v.m_normal);
  }
  template<typename P> Vertex2DTemplate<T> &setPos(const FixedSizeVectorTemplate<P,2> &pos) {
    m_pos = pos;
    return *this;
  }
  template<typename N> Vertex2DTemplate<T> &setNormal(const FixedSizeVectorTemplate<N,2> &normal) {
    m_normal = normal;
    return *this;
  }
  Vertex2DTemplate<T> &invertNormal() {
    m_normal = -m_normal;
    return *this;
  }
};

template<typename T> class Vertex2DTemplateArray : public CompactArray<Vertex2DTemplate<T> > {
public:
  Vertex2DTemplateArray() {
  }
  explicit Vertex2DTemplateArray(size_t capacity) : CompactArray(capacity) {
  }

  template<typename S> Vertex2DTemplateArray(const Vertex2DTemplateArray<S> &src) : CompactArray(src.size()) {
    const size_t n = src.size();
    for(const Vertex2DTemplate<S> v : src) {
      add(Vertex2DTemplate<T>(v));
    }
  }
  template<typename S> Vertex2DTemplateArray<T> &operator=(const Vertex2DTemplateArray<S> &src) {
    const size_t n = src.size();
    clear(n);
    if(n == 0) return *this;
    for(const Vertex2DTemplate<S> v : src) {
      add(Vertex2DTemplate<T>(v));
    }
    return *this;
  }
  Vertex2DTemplateArray<T> &invertNormals() {
    const size_t n = size();
    if(n == 0) return *this;
    for(Vertex2DTemplate<T> v : *this) {
      v.invertNormal();
    }
    return *this;
  }
};

typedef Vertex2DTemplate<float>      Vertex2D;
typedef Vertex2DTemplateArray<float> Vertex2DArray;
typedef Vertex2DArray                VertexCurve2D;

class VertexProfile2D : public Array<VertexCurve2D> {
public:
  VertexProfile2D() {
  }
  explicit VertexProfile2D(size_t capacity) : Array(capacity) {
  }
  VertexProfile2D &invertNormals();
};

// from MFCUtil library
class PolygonCurve2D;
class GlyphPolygon;
class GlyphCurveData;

class ProfileCurve2D {
public:
  // TT_PRIM_LINE, TT_PRIM_QSPLINE or TT_PRIM_CSPLINE
  short          m_type;
  Point2DArray   m_points;

  inline ProfileCurve2D(short type=0) {
    m_type = type;
  }

  ProfileCurve2D(const PolygonCurve2D &src);
  operator PolygonCurve2D() const;

  inline ProfileCurve2D &addPoint(const Point2D &p) {
    m_points.add(p); return *this;
  }
  inline ProfileCurve2D &addPoint(double x, double y) {
    return addPoint(Point2D(x,y));
  }
  inline Point2D &getLastPoint() {
    return m_points.last();
  }
  inline const Point2D &getLastPoint() const {
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
  Rectangle2D            getBoundingBox() const;
  const Point2DArray    &getAllPoints() const;
  Point2DRefArray        getAllPointsRef();
  void move(const Point2D &dp);
  String toString() const;
};

inline std::wostream &operator<<(std::wostream &out, const ProfileCurve2D &c) {
  return out << c.toString();
}

inline bool operator==(const ProfileCurve2D &p1, const ProfileCurve2D &p2) {
  return (p1.m_type == p2.m_type) && (p1.m_points == p2.m_points);
}

inline bool operator!=(const ProfileCurve2D &p1, const ProfileCurve2D &p2) {
  return !(p1==p2);
}

class ProfilePolygon2D {
private:
  VertexCurve2D getFlatVertexCurve() const;
  // Return noOfPoints normals
  VertexCurve2D getSmoothVertexCurve() const;
public:
  Point2D               m_start;
  Array<ProfileCurve2D> m_curveArray;
  bool                  m_closed;

  ProfilePolygon2D();
  ProfilePolygon2D(const GlyphPolygon &gp);
  inline void addCurve(const ProfileCurve2D &curve) {
    m_curveArray.add(curve);
  }

  Rectangle2D            getBoundingBox() const;
  Point2DArray           getAllPoints() const;
  Point2DRefArray        getAllPointsRef();
  Point2DArray           getCurvePoints() const;
  inline ProfileCurve2D &getLastCurve() {
    return m_curveArray.last();
  }

  inline const ProfileCurve2D &getLastCurve() const {
    return m_curveArray.last();
  }

  inline Point2D &getLastPoint() {
    return getLastCurve().getLastPoint();
  }

  inline const Point2D &getLastPoint() const {
    return getLastCurve().getLastPoint();
  }

  inline void removeLastPoint() {
    getLastCurve().removeLastPoint();
  }
  bool isEmpty() const;
  void move(const Point2D &dp);
  void reverseOrder();
  void apply(CurveOperator &op) const;

  inline VertexCurve2D getVertexCurve(bool smoothNormals) const {
    return smoothNormals ? getSmoothVertexCurve() : getFlatVertexCurve();
  }
  String toString() const;
};

inline bool operator==(const ProfilePolygon2D &p1, const ProfilePolygon2D &p2) {
  return p1.m_start      == p2.m_start
      && p1.m_closed     == p2.m_closed
      && p1.m_curveArray == p2.m_curveArray;
}

inline bool operator!=(const ProfilePolygon2D &p1, const ProfilePolygon2D &p2) {
  return !(p1==p2);
}

class Profile2D : public _PersistentData {
private:
  int findPolygonContainingPoint(const Point2D *p) const;
public:
  static const TCHAR *s_profileFileExtensions;

  Array<ProfilePolygon2D> m_polygonArray;
  void init();
  Profile2D() {
    init();
  }
  Profile2D(const String &fileName) {
    init();
    load(fileName);
  }
  Profile2D(const GlyphCurveData &glyphData, const String &name = s_defaultName);
  void addLine(const Point2D &p1, const Point2D &p2);
  // n is number of points. number of lines will be n-1
  void addLineStrip(const Point2D *points, int n);

  inline void addPolygon(const ProfilePolygon2D &polygon) {
    m_polygonArray.add(polygon);
  }
  ProfilePolygon2D &getLastPolygon() {
    return m_polygonArray.last();
  }
  bool                   isEmpty()          const;
  Rectangle2D            getBoundingBox()   const;
  Point2DArray           getAllPoints()     const;
  Point2DRefArray        getAllPointsRef();
  Point2DArray           getCurvePoints()   const;

  bool canConnect(const Point2D *p1, const Point2D *p2) const;
  void connect(   const Point2D *p1, const Point2D *p2);
  void move(      const Point2D &dp);
  void apply(CurveOperator &op) const;

  void putDataToDoc(  XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);

  VertexProfile2D        getVertexProfile(bool smoothNormals) const;

  String toString() const;

};

inline bool operator==(const Profile2D &p1, const Profile2D &p2) {
  return (p1.getName() == p2.getName()) && (p1.m_polygonArray == p2.m_polygonArray);
}

inline bool operator!=(const Profile2D &p1, const Profile2D &p2) {
  return !(p1==p2);
}

String selectAndValidateProfileFile();
