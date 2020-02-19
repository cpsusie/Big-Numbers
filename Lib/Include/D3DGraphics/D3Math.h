#pragma once

#include <MFCUtil/D3DeviceFactory.h>
#include <Math/MathLib.h>
#include <Math/Spherical.h>
#include <Math/Point2D.h>
#include "Math/Point3D.h"
#include "D3Error.h"

class ParametricSurface : public FunctionTemplate<Point2D, Point3D> {
};

float       operator*(            const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2);
float       length(               const D3DXVECTOR3 &v);
float       angle(                const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2);
// i = [0..2] giving ((1,0,0) or (0,1,0) or (0,0,1)
D3DXMATRIX &D3DXMatrixPerspectiveFov(D3DXMATRIX &mat , FLOAT              angel, FLOAT            apsect, FLOAT zn, FLOAT    fn, bool rightHanded);
D3DXMATRIX &D3DXMatrixLookAt(        D3DXMATRIX &view, const D3DXVECTOR3 &eye, const D3DXVECTOR3 &lookAt, const D3DXVECTOR3 &up, bool rightHanded);
D3DXVECTOR3 unitVector(           const D3DXVECTOR3 &v);
D3DXVECTOR3 createUnitVector(int i);
D3DXVECTOR3 rotate(               const D3DXVECTOR3 &v , const D3DXVECTOR3 &axis, float rad);
D3DXVECTOR3 crossProduct(         const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2);
D3DXVECTOR3 randomUnitVector();
D3DXVECTOR3 ortonormalVector(     const D3DXVECTOR3 &v);
String      toString(             const D3DXVECTOR3 &v, int dec = 3);
String      toString(             const D3DXVECTOR4 &v, int dec = 3);
String      toString(             const D3DXQUATERNION &q, int dec = 3);
D3DXMATRIX  transpose(            const D3DXMATRIX  &m);
D3DXMATRIX  invers(               const D3DXMATRIX  &m);
D3DXMATRIX  createIdentityMatrix();
D3DXMATRIX  createTranslateMatrix(const D3DXVECTOR3 &v);
D3DXMATRIX  createScaleMatrix(    const D3DXVECTOR3 &s);
D3DXMATRIX  createRotationMatrix( const D3DXVECTOR3 &axis, float rad);
D3DXVECTOR3 operator*(            const D3DXMATRIX  &m, const D3DXVECTOR3 &v);
D3DXVECTOR3 operator*(            const D3DXVECTOR3 &v, const D3DXMATRIX  &m);
// return quarternion that rotates x-axis (1,0,0) pointing into dir
D3DXQUATERNION createOrientation(const D3DXVECTOR3 &dir);

float       det(                  const D3DXMATRIX  &m);
String      toString(             const D3DXMATRIX  &m, int dec = 3);

#define D3DXORIGIN D3DXVECTOR3(0,0,0)

inline float dist(const D3DXVECTOR3 &p1, const D3DXVECTOR3 &p2) {
  return length(p1 - p2);
}

#define     radians(x) D3DXToRadian(x)
#define     degrees(x) D3DXToDegree(x)

#define A_POSITION_TRAITS(className)                                                                                                \
  template<typename TX,typename TY,typename TZ> className &setPos(const TX &_x, const TY &_y, const TZ &_z) {                       \
    x = (float)_x; y = (float)_y; z = (float)_z; return *this;                                                                      \
  }                                                                                                                                 \
  template<typename TV>                         className &setPos(const TV &v) {                                                    \
    x = (float)v.x; y = (float)v.y; z = (float)v.z; return *this;                                                                   \
  }                                                                                                                                 \
  template<typename T>                          className &setPos(const Point3DTemplate<T> &p) {                                    \
    x = (float)p.x; y = (float)p.y; z = (float)p.z; return *this;                                                                   \
  }                                                                                                                                 \
  inline Vertex operator-() const                        { return Vertex(     -x,-y,-z); }                                          \
  inline operator D3DXVECTOR3() const                    { return D3DXVECTOR3( x, y, z); }                                          \
  inline operator Point3D()     const                    { return Point3D(     x, y, z); }

#define A_NORMAL_TRAITS(className)                                                                                                  \
  template<typename TX,typename TY,typename TZ> className &setNormal(const TX &_x, const TX &_y, const TX &_z) {                    \
    nx = (float)_x ; ny = (float)_y ; nz = (float)_z; return *this;                                                                 \
  }                                                                                                                                 \
  template<typename TV> className &setNormal(const TV &n) {                                                                         \
    nx = (float)n.x; ny = (float)n.y; nz = (float)n.z; return *this;                                                                \
  }                                                                                                                                 \
  inline className &reverseNormal() { nx = -nx; ny = -ny; nz = -nz; return *this; }

#define E_NORMAL_TRAITS(className)                                                                                                  \
  template<typename TX,typename TY,typename TZ> className &setNormal(const TX &_x, const TX &_y, const TX &_z) { return *this; }    \
  template<typename TV>                         className &setNormal(const TV &n)                              { return *this; }    \
  inline                                        className &reverseNormal()                                     { return *this; }

#define A_DIFFUSE_TRAITS(className)                                                                                                 \
  inline                                        className &setDiffuse(D3DCOLOR _diffuse) {                                          \
    diffuse = _diffuse; return *this;                                                                                               \
  }

#define E_DIFFUSE_TRAITS(className)                                                                                                 \
  inline                                        className &setDiffuse(D3DCOLOR _diffuse)                       { return *this; }

#define A_TEXTURE_TRAITS(className)                                                                                                 \
  template<typename TU, typename TV>            className &setTexture(const TU &_tu, const TV &_tv) {                               \
    tu = (float)_tu; tv = (float)_tv; return *this;                                                                                 \
  }                                                                                                                                 \
  inline                                        className &setTexture(const TextureVertex &p) {                                     \
    tu = p.u; tv = p.v; return *this;                                                                                               \
  }                                                                                                                                 \
  template<typename T>                          className &setTexture(const T &p) {                                                 \
    tu = (float)p.x ; tv = (float)p.y; return *this;                                                                                \
  }

#define E_TEXTURE_TRAITS(className)                                                                                                 \
  template<typename TU, typename TV>            className &setTexture(const TU &_tu, const TV &_tv)            { return *this; }    \
  template<typename T>                          className &setTexture(const T &p)                              { return *this; }

#define SETFVFDATA_TRAITS                                                                                       \
  inline void setFVFData(const Vertex &v, const Vertex &n, D3DCOLOR diffuse, const TextureVertex &tv) {         \
    setPos(v); setNormal(n); setDiffuse(diffuse); setTexture(tv);                                               \
  }

#define FVF_TRAITS(className, p,n,d,t) \
  p##_POSITION_TRAITS(className)       \
  n##_NORMAL_TRAITS(  className)       \
  d##_DIFFUSE_TRAITS( className)       \
  t##_TEXTURE_TRAITS( className)

class TextureVertex {
public:
  float u, v;
  inline TextureVertex() {}
  template<typename TU, typename TV> TextureVertex(TU _u, TV _v) : u((float)_u), v((float)_v) {
  }
  inline TextureVertex(const D3DXVECTOR2 &vec) : u(vec.x), v(vec.y) {
  }
  template<typename T> TextureVertex(const Point2DTemplate<T> &p) : u((float)p.x), v((float)p.y) {
  }
};

inline String toString(const TextureVertex &tv, int dec = 3) {
  return format(_T("(%*.*f,%*.*f)"), dec+3,dec, tv.u, dec+3,dec, tv.v);
}

class Vertex {
public:
  float x, y, z;
  enum FVF {
    FVF_Flags = D3DFVF_XYZ
  };
  inline Vertex() {
  }
  template<typename TX, typename TY, typename TZ> Vertex(TX _x, TY _y, TZ _z) : x((float)_x), y((float)_y), z((float)_z) {
  }
  template<typename T> Vertex(const Point3DTemplate<T> &p) : x((float)p.x), y((float)p.y), z((float)p.z) {
  }
  inline Vertex(const D3DXVECTOR3 &v) : x(v.x), y(v.y), z(v.z) {
  }

  FVF_TRAITS(Vertex,A,E,E,E)
};

class VertexNormal {
public:
  float x, y, z;
  float nx, ny, nz;
  enum FVF {
    FVF_Flags = D3DFVF_XYZ | D3DFVF_NORMAL
  };

  FVF_TRAITS(VertexNormal,A,A,E,E)
};

class VertexDiffuse {
public:
  float x, y, z;
  DWORD diffuse;
  enum FVF {
    FVF_Flags = D3DFVF_XYZ | D3DFVF_DIFFUSE
  };
  FVF_TRAITS(VertexDiffuse, A,E,A,E)
};

class VertexNormalDiffuse {
public:
  float x, y, z;
  float nx, ny, nz;
  DWORD diffuse;
  enum FVF {
    FVF_Flags = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE
  };
  FVF_TRAITS(VertexNormalDiffuse,A,A,A,E)
};

class VertexNormalTex1 {
public:
  float x, y, z;
  float nx, ny, nz;
  float tu, tv;
  enum FVF {
    FVF_Flags = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1
  };
  FVF_TRAITS(VertexNormalTex1,A,A,E,A)
};

class VertexTex1 {
public:
  float x, y, z;
  float tu, tv;
  enum FVF {
      FVF_Flags = D3DFVF_XYZ | D3DFVF_TEX1
  };
  FVF_TRAITS(VertexTex1, A,E,E,A)
};

class VertexDiffuseTex1 {
public:
  float x, y, z;
  DWORD diffuse;
  float tu, tv;
  enum FVF {
    FVF_Flags = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1
  };
  FVF_TRAITS(VertexDiffuseTex1,A,E,A,A)
};

class VertexNormalDiffuseTex1 {
public:
  float x, y, z;
  float nx, ny, nz;
  DWORD diffuse;
  float tu, tv;
  enum FVF {
    FVF_Flags = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1
  };
  FVF_TRAITS(VertexNormalDiffuseTex1,A,A,A,A)
};

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
  template<typename TX, typename TY, typename TZ> Point3DP(TX x, TY y, TZ z) : Point3D((double)x, (double)y, (double)z) {
  }
  template<typename T> Point3DP(const Point3DTemplate<T> &p) : Point3D(p) {
  }
  template<typename T> Point3DP(const Size3DTemplate<T> &s) : Point3D(s) {
  }

  inline Point3DP(const D3DXVECTOR3 &v) : Point3D(v.x, v.y, v.z) {
  }
  inline operator D3DXVECTOR3() const {
    return D3DXVECTOR3((float)x, (float)y, (float)z);
  }
};

class D3World {
private:
  D3DXVECTOR3    m_scale, m_pos;
  D3DXQUATERNION m_q;

  static inline void decomposeMatrix(D3DXVECTOR3 &scale, D3DXQUATERNION &q, D3DXVECTOR3 &pos, const D3DXMATRIX &mat) {
    FV(D3DXMatrixDecompose(&scale, &q, &pos, &mat));
  }
  static inline void composeMatrix(const D3DXVECTOR3 &scale, const D3DXQUATERNION &q, const D3DXVECTOR3 &pos, D3DXMATRIX &mat) {
    D3DXMatrixTransformation(&mat, NULL, NULL, &scale, NULL, &q, &pos);
  }

public:
  D3World() {
    reset();
  }
  D3World(const D3DXMATRIX &world) {
    decomposeMatrix(m_scale, m_q, m_pos, world);
  }
  operator D3DXMATRIX() const {
    D3DXMATRIX world;
    composeMatrix(m_scale, m_q, m_pos, world);
    return world;
  }
  inline D3World &reset() {
    D3DXMATRIX id;
    D3DXMatrixIdentity(&id);
    decomposeMatrix(m_scale, m_q, m_pos, id);
    return *this;
  }
  inline D3World &resetPos() {
    return setPos(D3DXORIGIN);
  }
  inline D3World &resetOrientation() {
    D3DXQuaternionIdentity(&m_q);
    return *this;
  }
  inline D3World &resetScale() {
    return setScaleAll(1);
  }
  inline D3World &setPos(const D3DXVECTOR3 &pos) {
    m_pos = pos;
    return *this;
  }
  inline const D3DXVECTOR3 &getPos() const {
    return m_pos;
  }
  inline D3World &setScale(const D3DXVECTOR3 &scale) {
    m_scale = scale;
    return *this;
  }
  inline const D3DXVECTOR3 &getScale() const {
    return m_scale;
  }
  inline D3World &setScaleAll(float scale) {
    return setScale(D3DXVECTOR3(scale, scale, scale));
  }
  inline D3World &setOrientation(const D3DXQUATERNION &q) {
    m_q = q;
    return *this;
  }
  inline const D3DXQUATERNION &getOrientation() const {
    return m_q;
  }
  String toString(int dec = 3) const {
    return format(_T("Pos:%s, Orientation:%s, Scale:%s")
                 ,::toString(m_pos  ,dec).cstr()
                 ,::toString(m_q    ,dec).cstr()
                 ,::toString(m_scale,dec).cstr()
                 );
  }
};

class D3PosDirUpScale {
private:
  bool        m_rightHanded;
  D3DXVECTOR3 m_pos, m_dir, m_up, m_scale;
  D3DXMATRIX  m_view;
  D3PosDirUpScale &updateView();
public:
  D3PosDirUpScale(bool rightHanded);

  inline bool getRightHanded() const {
    return m_rightHanded;
  }
  // return *this
  D3PosDirUpScale &setRightHanded(bool rightHanded);
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
  // set Position to (0,0,0)
  inline D3PosDirUpScale &resetPos() {
    return setPos(getDefaultPos());
  }
  // set scale in all dimensions to 1
  inline D3PosDirUpScale &resetScale() {
    return setScaleAll(1);
  }
  // Return vector (0,0,0)
  static D3DXVECTOR3 getDefaultPos()   { return D3DXORIGIN;           }
  // Return vector (0,0,0)
  static D3DXVECTOR3 getDefaultScale() { return D3DXVECTOR3(1, 1, 1); }
  // Return vector (0,1,0)
  static D3DXVECTOR3 getDefaultDir()   { return D3DXVECTOR3(0,0,-1);   }
  // Return vector (0,0,1)
  static D3DXVECTOR3 getDefaultUp()    { return D3DXVECTOR3(0,1,0);   }
  // Set dir=(0,1,0), up=(0,0,1)
  inline D3PosDirUpScale &resetOrientation() { return setOrientation(getDefaultDir(), getDefaultUp()); }

  D3PosDirUpScale &setWorldMatrix(const D3DXMATRIX &world);

  inline D3DXMATRIX getWorldMatrix() const {
    D3DXMATRIX result;
    return *D3DXMatrixScaling(&result, m_scale.x, m_scale.y, m_scale.z) * invers(m_view);
  }
  inline const D3DXMATRIX &getViewMatrix() const {
    return m_view;
  }
  inline D3DXMATRIX        getRotationMatrix() const {
    return D3PosDirUpScale(*this).resetPos().resetScale().getWorldMatrix();
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
    return (m_pos            == pdus.m_pos           )
        && (m_dir            == pdus.m_dir           )
        && (m_up             == pdus.m_up            )
        && (m_scale          == pdus.m_scale         )
        && (getRightHanded() == pdus.getRightHanded());
  }
  inline bool operator!=(const D3PosDirUpScale &pdus) const {
    return !(*this == pdus);
  }
};

class D3Spherical : public FloatSpherical {
public:
  inline D3Spherical() : FloatSpherical() {
  }
  template<typename TP> D3Spherical(const Point3DTemplate<TP> &p) : FloatSpherical(p) {
  }
  template<typename X, typename Y, typename Z> D3Spherical(const X &x, const Y &y, const Z &z) : FloatSpherical(x,y,z) {
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
