#pragma once

#include <MFCUtil/D3DeviceFactory.h>
#include <Math/MathLib.h>
#include <Math/Spherical.h>
#include <Math/Point2D.h>
#include "Math/Point3D.h"
#include "D3Error.h"

//#define LEFTHANDED

#ifdef LEFTHANDED

#define D3DXMatrixPerspectiveFov(mat , angel, apsect, zn, fn)  D3DXMatrixPerspectiveFovLH(mat,angel,apsect,zn,fn)
#define D3DXMatrixLookAt(        view, pos  , lookAt, up)      D3DXMatrixLookAtLH(view, pos, lookAt, up)

#else

#define D3DXMatrixPerspectiveFov(mat , angel, apsect, zn, fn)  D3DXMatrixPerspectiveFovRH(mat,angel,apsect,zn,fn)
#define D3DXMatrixLookAt(        view, pos  , lookAt, up)      D3DXMatrixLookAtRH(view, pos, lookAt, up)

#endif


class ParametricSurface : public FunctionTemplate<Point2D, Point3D> {
};

float       operator*(            const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2);
float       length(               const D3DXVECTOR3 &v);
float       angle(                const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2);
// i = [0..2] giving ((1,0,0) or (0,1,0) or (0,0,1)
D3DXVECTOR3 unitVector(           const D3DXVECTOR3 &v);
D3DXVECTOR3 createUnitVector(int i);
D3DXVECTOR3 rotate(               const D3DXVECTOR3 &v , const D3DXVECTOR3 &axis, double rad);
D3DXVECTOR3 crossProduct(         const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2);
D3DXVECTOR3 randomUnitVector();
D3DXVECTOR3 ortonormalVector(     const D3DXVECTOR3 &v);
String      toString(             const D3DXVECTOR3 &v, int dec = 3);
String      toString(             const D3DXVECTOR4 &v, int dec = 3);
D3DXMATRIX  transpose(            const D3DXMATRIX  &m);
D3DXMATRIX  invers(               const D3DXMATRIX  &m);
D3DXMATRIX  createIdentityMatrix();
D3DXMATRIX  createTranslateMatrix(const D3DXVECTOR3 &v);
D3DXMATRIX  createScaleMatrix(    const D3DXVECTOR3 &s);
D3DXMATRIX  createRotationMatrix( const D3DXVECTOR3 &axis, double rad);
D3DXVECTOR3 operator*(            const D3DXMATRIX  &m, const D3DXVECTOR3 &v);
D3DXVECTOR3 operator*(            const D3DXVECTOR3 &v, const D3DXMATRIX  &m);
float       det(                  const D3DXMATRIX  &m);
String      toString(             const D3DXMATRIX  &m, int dec = 3);

#define D3DXORIGIN D3DXVECTOR3(0,0,0)

inline float dist(const D3DXVECTOR3 &p1, const D3DXVECTOR3 &p2) {
  return length(p1 - p2);
}

#define     radians(x) D3DXToRadian(x)
#define     degrees(x) D3DXToDegree(x)

#define POSITION_TRAITS                                                                                         \
  inline void setPos(   double _x, double _y, double _z) { x  = (float)_x ; y  = (float)_y ; z  = (float)_z ; } \
  inline void setPos(   const Vertex      &v)            { x  = v.x       ; y  = v.y       ; z  = v.z       ; } \
  inline void setPos(   const D3DXVECTOR3 &v)            { x  = v.x       ; y  = v.y       ; z  = v.z       ; } \
  inline void setPos(   const Point3D     &p)            { x  = (float)p.x; y  = (float)p.y; z  = (float)p.z; } \
  inline Vertex operator-() const                        { return Vertex(     -x,-y,-z); }                      \
  inline operator D3DXVECTOR3() const                    { return D3DXVECTOR3( x, y, z); }                      \
  inline operator Point3D()     const                    { return Point3D(     x, y, z); }

#define NORMAL_TRAITS                                                                                           \
  inline void setNormal(double _x, double _y, double _z) { nx = (float)_x ; ny = (float)_y ; nz = (float)_z ; } \
  inline void setNormal(const Vertex                 &n) { nx = n.x       ; ny = n.y       ; nz = n.z       ; } \
  inline void setNormal(const D3DXVECTOR3            &n) { nx = n.x       ; ny = n.y       ; nz = n.z       ; } \
  inline void setNormal(const Point3D                &n) { nx = (float)n.x; ny = (float)n.y; nz = (float)n.z; } \
  inline void reverseNormal() { nx*=-1; ny*=-1; nz*=-1; }

#define EMPTY_NORMAL_TRAITS inline void setNormal(const Vertex &n) { ; /* do nothing */ }

#define DIFFUSE_TRAITS \
  inline void setDiffuse(D3DCOLOR _diffuse) { diffuse = _diffuse; }

#define EMPTY_DIFFUSE_TRAITS inline void setDiffuse(D3DCOLOR _diffuse) { ; /* do nothing */ }

#define TEXTURE_TRAITS                                                                                          \
  inline void setTexture(double _tu, double _tv        ) { tu = (float)_tu ; tv = (float)_tv; }                 \
  inline void setTexture(const TextureVertex &p        ) { tu = p.u        ; tv = p.v       ; }                 \
  inline void setTexture(const D3DXVECTOR2   &p        ) { tu = (float)p.x ; tv = (float)p.y; }                 \
  inline void setTexture(const Point2D       &p        ) { tu = (float)p.x ; tv = (float)p.y; }

#define EMPTY_TEXTURE_TRAITS inline void setTexture(const TextureVertex &p) { ;/* do nothing */ }

#define SETFVFDATA_TRAITS                                                                                       \
  inline void setFVFData(const Vertex &v, const Vertex &n, D3DCOLOR diffuse, const TextureVertex &tv) {         \
    setPos(v); setNormal(n); setDiffuse(diffuse); setTexture(tv);                                               \
  }

class TextureVertex {
public:
  float u, v;
  inline TextureVertex() {}
  inline TextureVertex(float  _u, float  _v ) : u(_u), v(_v) {
  }
  inline TextureVertex(double _u, double _v ) : u((float)_u), v((float)_v) {
  }
  inline TextureVertex(const D3DXVECTOR2 &_v) : u(_v.x), v(_v.y) {
  }
  inline TextureVertex(const Point2D     &p ) : u((float)p.x), v((float)p.y) {
  }
};

inline String toString(const TextureVertex &tv, int dec = 3) {
  return format(_T("(%*.*f,%*.*f)"), dec+3,dec, tv.u, dec+3,dec, tv.v);
}

typedef struct Vertex {
  float x, y, z;
  enum FVF {
    FVF_Flags = D3DFVF_XYZ
  };
  inline Vertex() {
  }
  inline Vertex(int    _x, int    _y, int    _z) { x  = (float)_x ; y  = (float)_y ; z  = (float)_z ; }
  inline Vertex(float  _x, float  _y, float  _z) { x  = _x        ; y  = _y        ; z  = _z        ; }
  inline Vertex(double _x, double _y, double _z) { x  = (float)_x ; y  = (float)_y ; z  = (float)_z ; }
  inline Vertex(     const Point3D     &p)       { x  = (float)p.x; y  = (float)p.y; z  = (float)p.z; }
  inline Vertex(     const D3DXVECTOR3 &v)       { x  = v.x       ; y  = v.y       ; z  = v.z       ; }

  POSITION_TRAITS
  EMPTY_NORMAL_TRAITS
  EMPTY_DIFFUSE_TRAITS
  EMPTY_TEXTURE_TRAITS
} Vertex;

typedef struct {
  float x, y, z;
  float nx, ny, nz;
  enum FVF {
    FVF_Flags = D3DFVF_XYZ | D3DFVF_NORMAL
  };
  POSITION_TRAITS
  NORMAL_TRAITS
  EMPTY_DIFFUSE_TRAITS
  EMPTY_TEXTURE_TRAITS
} VertexNormal;

typedef struct {
  float x, y, z;
  DWORD diffuse;
  enum FVF {
    FVF_Flags = D3DFVF_XYZ | D3DFVF_DIFFUSE
  };
  POSITION_TRAITS
  EMPTY_NORMAL_TRAITS
  DIFFUSE_TRAITS
  EMPTY_TEXTURE_TRAITS
} VertexDiffuse;

typedef struct {
  float x, y, z;
  float nx, ny, nz;
  DWORD diffuse;
  enum FVF {
    FVF_Flags = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE
  };
  POSITION_TRAITS
  NORMAL_TRAITS
  DIFFUSE_TRAITS
  EMPTY_TEXTURE_TRAITS
} VertexNormalDiffuse;

typedef struct {
  float x, y, z;
  float nx, ny, nz;
  float tu, tv;
  enum FVF {
      FVF_Flags = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1
  };
  POSITION_TRAITS
  NORMAL_TRAITS
  EMPTY_DIFFUSE_TRAITS
  TEXTURE_TRAITS
} VertexNormalTex1;

typedef struct {
  float x, y, z;
  float tu, tv;
  enum FVF {
      FVF_Flags = D3DFVF_XYZ | D3DFVF_TEX1
  };
  POSITION_TRAITS
  EMPTY_NORMAL_TRAITS
  EMPTY_DIFFUSE_TRAITS
  TEXTURE_TRAITS
} VertexTex1;

typedef struct {
  float x, y, z;
  DWORD diffuse;
  float tu, tv;
  enum FVF {
      FVF_Flags = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1
  };
  POSITION_TRAITS
  EMPTY_NORMAL_TRAITS
  DIFFUSE_TRAITS
  TEXTURE_TRAITS
} VertexDiffuseTex1;

typedef struct {
  float x, y, z;
  float nx, ny, nz;
  DWORD diffuse;
  float tu, tv;
  enum FVF {
      FVF_Flags = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1
  };
  POSITION_TRAITS
  NORMAL_TRAITS
  DIFFUSE_TRAITS
  TEXTURE_TRAITS
} VertexNormalDiffuseTex1;

int    FVFToSize(   DWORD     fvf);
int    formatToSize(D3DFORMAT f  );

class Line3D {
public:
  Vertex m_p1, m_p2;
  inline Line3D() {}
  inline Line3D(const Vertex &p1, const Vertex &p2) : m_p1(p1), m_p2(p2) {
  }
  inline Line3D(const Point3D &p1, const Point3D &p2) : m_p1(p1), m_p2(p2) {
  }
};

typedef CompactArray<Vertex>        VertexArray;
typedef CompactArray<TextureVertex> TextureVertexArray;
class CurveArray : public Array<VertexArray> {
public:
  static CurveArray createSphereObject(double r);
};

class Point3DP : public Point3D {
public:
  inline Point3DP() {
  }
  template<typename X, typename Y, typename Z> Point3DP(X x, Y y, Z z) : Point3D((double)x, (double)y, (double)z) {
  }
  template<typename T> Point3DP(const Point3DTemplate<T> &p) : Point3D((double)p.x, (double)p.y, (double)p.z) {
  }
  inline Point3DP(const D3DXVECTOR3 &v) : Point3D(v.x, v.y, v.z) {
  }
  inline operator D3DXVECTOR3() const {
    return D3DXVECTOR3((float)x, (float)y, (float)z);
  }
};

class D3PosDirUpScale {
private:
  D3DXVECTOR3 m_pos, m_dir, m_up, m_scale;
  D3DXMATRIX  m_view;
  D3PosDirUpScale &updateView();
public:
  D3PosDirUpScale();
  inline const D3DXVECTOR3 &getPos() const {
    return m_pos;
  }
  inline const D3DXVECTOR3 &getDir() const {
    return m_dir;
  }
  inline const D3DXVECTOR3 &getUp() const {
    return m_up;
  }
  inline D3DXVECTOR3 getRight() const {
    return crossProduct(m_dir, m_up);
  }
  inline const D3DXVECTOR3 &getScale() const {
    return m_scale;
  }
  D3PosDirUpScale &setPos(const D3DXVECTOR3 &pos) {
    m_pos = pos;
    return updateView();
  }
  D3PosDirUpScale &setOrientation( const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up);
  inline D3PosDirUpScale &setScale(const D3DXVECTOR3 &scale) {
    m_scale = scale;
    return *this;
  }
  inline D3PosDirUpScale &setScaleAll(float scale) {
    return setScale(D3DXVECTOR3(scale, scale, scale));
  }
  D3PosDirUpScale &setWorldMatrix(const D3DXMATRIX &world);

  inline D3DXMATRIX getWorldMatrix() const {
    D3DXMATRIX result;
    return *D3DXMatrixScaling(&result, m_scale.x, m_scale.y, m_scale.z) * invers(m_view);
  }
  inline const D3DXMATRIX &getViewMatrix() const {
    return m_view;
  }
  inline D3DXMATRIX        getRotationMatrix() const {
    return D3PosDirUpScale(*this).setPos(D3DXVECTOR3(0,0,0)).setScale(D3DXVECTOR3(1,1,1)).getWorldMatrix();
  }
  inline D3DXQUATERNION    getQuarternion() const {
    D3DXQUATERNION result;
    return *D3DXQuaternionRotationMatrix(&result, &getRotationMatrix());
  }
  inline D3DXMATRIX        getScaleMatrix()    const {
    D3DXMATRIX result;
    return *D3DXMatrixScaling(&result, m_scale.x, m_scale.y, m_scale.z);
  }

  String toString(int dec=3) const;
  inline bool operator==(const D3PosDirUpScale &pdus) const {
    return (m_pos == pdus.m_pos) && (m_dir == pdus.m_dir) && (m_up == pdus.m_up) && (m_scale == pdus.m_scale);
  }
  inline bool operator!=(const D3PosDirUpScale &pdus) const {
    return !(*this == pdus);
  }
};

class D3Ray {
public:
  D3DXVECTOR3 m_orig; // Point in world space where ray starts
  D3DXVECTOR3 m_dir;  // direction of ray in world space
  inline D3Ray() : m_orig(0, 0, 0), m_dir(0, 0, 0) {
  }
  inline D3Ray(const D3DXVECTOR3 &orig, const D3DXVECTOR3 &m_dir) : m_orig(orig), m_dir(unitVector(m_dir)) {
  }
  inline void clear() {
    m_dir = m_orig = D3DXVECTOR3(0,0,0);
  }
  inline bool isSet() const {
    return length(m_dir) > 0;
  }
  inline String toString(int dec=3) const {
    return format(_T("Orig:%s, Dir:%s"), ::toString(m_orig,dec).cstr(), ::toString(m_dir,dec).cstr());
  }
};

class D3Spherical : public Spherical {
public:
  inline D3Spherical() : Spherical() {
  }
  inline D3Spherical(const Point3D &p) : Spherical(p) {
  }
  inline D3Spherical(double x, double y, double z) : Spherical(x,y,z) {
  }
  inline D3Spherical(const D3DXVECTOR3 &v) {
    init(v.x, v.y, v.z);
  }
  operator D3DXVECTOR3() const;
};

typedef enum {
  PP_2DFUNCTION
 ,PP_PARAMETRICSURFACE
 ,PP_ISOSURFACE
 ,PP_ISOCURVE
} PersistentDataType;


#include "PragmaLib.h"
