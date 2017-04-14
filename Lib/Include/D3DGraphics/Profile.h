#pragma once

#include <MFCUtil/ShapeFunctions.h>
#include <MFCUtil/PolygonCurve.h>
#include "D3Math.h"
#include "MeshBuilder.h"

class ProfileRotationParameters {
public:
  ProfileRotationParameters(int rotateAxis=0, int alignx=0, int aligny=1, double rad=2*D3DX_PI, int edgeCount=20, int smoothness=0, bool useColor=false, D3DCOLOR color=0);
  int      m_rotateAxis;
  int      m_alignx;
  int      m_aligny;
  double   m_rad;
  int      m_edgeCount;
  int      m_smoothness;
  bool     m_useColor;
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

#define ROTATESMOOTH 1
#define NORMALSMOOTH 2

class ProfileCurve {
public:
  short          m_type; // TT_PRIM_LINE, TT_PRIM_QSPLINE or TT_PRIM_CSPLINE
  Point2DArray   m_points;

  inline ProfileCurve(short type) {
    m_type = type;
  }

  ProfileCurve(const PolygonCurve &src);

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
  String toXML();
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

  Rectangle2D     getBoundingBox() const;
  Point2DArray    getAllPoints() const;
  CompactArray<Point2D*> getAllPointsRef();
  Point2DArray    getCurvePoints() const;
  Point2DArray    getFlatNormals() const; // return noOfLines normals
  Point2DArray    getSmoothNormals() const; // return noOfPoints normals
  
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
  String toXML();
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
  Profile(const String &xml, const String &name="");
  bool hasDefaultName() const;
  String getDisplayName() const;

  void addLine(const Point2D &p1, const Point2D &p2);
  void addLineStrip(const Point2D *points, int n); // n is number of points. number of lines will be n-1

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
  Point2DArray    getFlatNormals() const; // return noOfLines normals
  Point2DArray    getSmoothNormals() const; // return noOfPoints normals
  bool canConnect(const Point2D *p1, const Point2D *p2) const;
  void connect(const Point2D *p1, const Point2D *p2);
  void move(const Point2D &dp);
  void apply(CurveOperator &op) const;
  String toString() const;
  String toXML();
  void parseXML(const String &xml);
  void read( FILE *f);
  void write(FILE *f);
  void load(const String &fileName);
  void save(const String &fileName);
};

bool operator==(const ProfileCurve   &p1, const ProfileCurve   &p2);
bool operator!=(const ProfileCurve   &p1, const ProfileCurve   &p2);
bool operator==(const ProfilePolygon &p1, const ProfilePolygon &p2);
bool operator!=(const ProfilePolygon &p1, const ProfilePolygon &p2);
bool operator==(const Profile        &p1, const Profile        &p2);
bool operator!=(const Profile        &p1, const Profile        &p2);

LPD3DXMESH rotateProfile(AbstractMeshFactory &amf, const Profile &profile, const ProfileRotationParameters &param, bool doubleSided);
