#pragma once

#include <FlagTraits.h>
#include <MFCUtil/ShapeFunctions.h>
#include "D3Math.h"
#include "MeshBuilder.h"

#define PRROT_ROTATESMOOTH      0x01
#define PRROT_NORMALSMOOTH      0x02
#define PRROT_INVERTORIENTATION 0x04
#define PRROT_INVERTNORMALS     0x08
#define PRROT_USECOLOR          0x10

class ProfileRotationParameters {
public:
  ProfileRotationParameters(int rotateAxis=0, int alignx=0, int aligny=1, double rad=2*D3DX_PI, int edgeCount=20, BYTE flags=0, D3DCOLOR color=0);
  int      m_rotateAxis;
  int      m_alignx;
  int      m_aligny;
  double   m_rad;
  int      m_edgeCount;
  FLAGTRAITS(ProfileRotationParameters, BYTE, m_flags);
  D3DCOLOR m_color;
};

class ProfileStretchParameters {
public:
  ProfileStretchParameters();
  D3DXVECTOR3 d;
};

bool operator==(const ProfileRotationParameters &p1, const ProfileRotationParameters &p2);
bool operator!=(const ProfileRotationParameters &p1, const ProfileRotationParameters &p2);
bool operator==(const ProfileStretchParameters  &p1, const ProfileStretchParameters  &p2);
bool operator!=(const ProfileStretchParameters  &p1, const ProfileStretchParameters  &p2);

template<typename T> class Vertex2DTemplate {
public:
  Point2DTemplate<T> m_pos;
  Point2DTemplate<T> m_normal;
  Vertex2DTemplate() {
  }
  template<typename V> Vertex2DTemplate(const Vertex2DTemplate<V> &v) : m_pos(v.pos), m_normal(v.normal) {
  }
  template<typename P, typename N> Vertex2DTemplate(const Point2DTemplate<P> &pos, const Point2DTemplate<N> &normal) : m_pos(pos), m_normal(normal) {
  }
  template<typename V> Vertex2DTemplate<T> &operator=(const Vertex2DTemplate<V> &v) {
    return setPos(v.pos).setNormal(v.m_normal);
  }
  template<typename P> Vertex2DTemplate<T> &setPos(Point2DTemplate<P> &pos) {
    m_pos = pos;
    return *this;
  }
  template<typename N> Vertex2DTemplate<T> &setNormal(Point2DTemplate<N> &normal) {
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

  template<typename S> Vertex2DTemplateArray(const Vertex2DTemplateArray<S> &src) : CompactArray(src.size) {
    const size_t n = src.size();
    for(const Vertex2DTemplate<S> *srcp = src.getBuffer(); *endp = srcp + n, srcp < endp;) {
      add(Vertex2DTemplate<T>(*(srcp++)));
    }
  }
  template<typename S> Vertex2DTemplateArray<T> &operator=(const Vertex2DTemplateArray<S> &src) {
    const size_t n = src.size();
    clear(n);
    if(n == 0) return *this;
    for(const Vertex2DTemplate<S> *srcp = src.getBuffer(), *endp = srcp + n; srcp < endp;) {
      add(Vertex2DTemplate<T>(*(srcp++)));
    }
    return *this;
  }
  Vertex2DTemplateArray<T> &invertNormals() {
    const size_t n = size();
    if(n == 0) return *this;
    for(Vertex2DTemplate<T> *vp = &first(), *endp = vp + n; vp < endp;) {
      (vp++)->invertNormal();
    }
    return *this;
  }
};

typedef Vertex2DTemplate<float>      Vertex2D;
typedef Vertex2DTemplateArray<float> Vertex2DArray;

class PolygonCurve;

class ProfileCurve {
public:
  // TT_PRIM_LINE, TT_PRIM_QSPLINE or TT_PRIM_CSPLINE
  short          m_type;
  Point2DArray   m_points;

  inline ProfileCurve(short type=0) {
    m_type = type;
  }

  ProfileCurve(const PolygonCurve &src);
  operator PolygonCurve() const;

  inline ProfileCurve &addPoint(const Point2D &p) {
    m_points.add(p); return *this;
  }
  inline ProfileCurve &addPoint(double x, double y) {
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
  Rectangle2D getBoundingBox() const;
  Point2DArray getAllPoints() const;
  CompactArray<Point2D*> getAllPointsRef();
  void move(const Point2D &dp);
  String toString() const;
};

class ProfilePolygon {
public:
  Point2D             m_start;
  Array<ProfileCurve> m_curveArray;
  bool                m_closed;

  ProfilePolygon();

  inline void addCurve(const ProfileCurve &curve) {
    m_curveArray.add(curve);
  }

  Rectangle2D          getBoundingBox() const;
  Point2DArray         getAllPoints() const;
  CompactArray<Point2D*> getAllPointsRef();
  Point2DArray         getCurvePoints() const;
  Vertex2DArray        getFlatVertexArray() const;
  // Return noOfPoints normals
  Vertex2DArray        getSmoothVertexArray() const;
  inline Vertex2DArray getAllVertices(bool smoothNormals) const {
    return smoothNormals ? getSmoothVertexArray() : getFlatVertexArray();
  }
  inline ProfileCurve &getLastCurve() {
    return m_curveArray.last();
  }

  inline const ProfileCurve &getLastCurve() const {
    return m_curveArray.last();
  }

  inline Point2D &getLastPoint() {
    return getLastCurve().getLastPoint();
  }

  inline const Point2D &getLastPoint() const {
    return getLastCurve().getLastPoint();
  }

  bool isEmpty() const;
  void move(const Point2D &dp);
  void reverseOrder();
  void apply(CurveOperator &op) const;

  String toString() const;
};

class Profile {
private:
  int findPolygonContainingPoint(const Point2D *p) const;
public:
  String                m_name;
  Array<ProfilePolygon> m_polygonArray;
  void init();
  Profile() {
    init();
  }
  bool hasDefaultName() const;
  String getDisplayName() const;

  void addLine(const Point2D &p1, const Point2D &p2);
  // n is number of points. number of lines will be n-1
  void addLineStrip(const Point2D *points, int n);

  inline void addPolygon(const ProfilePolygon &polygon) {
    m_polygonArray.add(polygon);
  }
  ProfilePolygon &getLastPolygon() {
    return m_polygonArray.last();
  }
  bool            isEmpty() const;
  Rectangle2D     getBoundingBox() const;
  Point2DArray    getAllPoints() const;
  CompactArray<Point2D*> getAllPointsRef();
  Point2DArray    getCurvePoints() const;
  // Return noOfLines normals
  Vertex2DArray   getFlatVertexArray() const;
  // Return noOfPoints normals
  Vertex2DArray   getSmoothVertexArray() const;
  inline Vertex2DArray getAllVertices(bool smoothNormals) const {
    return smoothNormals ? getSmoothVertexArray() : getFlatVertexArray();
  }

  bool canConnect(const Point2D *p1, const Point2D *p2) const;
  void connect(const Point2D *p1, const Point2D *p2);
  void move(const Point2D &dp);
  void apply(CurveOperator &op) const;
  String toString() const;
};

bool operator==(const ProfileCurve   &p1, const ProfileCurve   &p2);
bool operator!=(const ProfileCurve   &p1, const ProfileCurve   &p2);
bool operator==(const ProfilePolygon &p1, const ProfilePolygon &p2);
bool operator!=(const ProfilePolygon &p1, const ProfilePolygon &p2);
bool operator==(const Profile        &p1, const Profile        &p2);
bool operator!=(const Profile        &p1, const Profile        &p2);

LPD3DXMESH rotateProfile(AbstractMeshFactory &amf, const Profile &profile, const ProfileRotationParameters &param, bool doubleSided);
