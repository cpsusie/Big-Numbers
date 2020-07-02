#pragma once

#include <Random.h>
#include <MFCUtil/DirectXDeviceFactory.h>
#include <Math/MathLib.h>
#include <Math/Point2D.h>
#include "Math/Point3D.h"

float              operator*(            const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2);
float              length(               const D3DXVECTOR3 &v);
float              angle(                const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2);
D3DXVECTOR3        unitVector(           const D3DXVECTOR3 &v);
extern D3DXVECTOR3 E[3];
// i = [0..2] giving ((1,0,0) or (0,1,0) or (0,0,1)
D3DXVECTOR3        createUnitVector(UINT i);
D3DXVECTOR3        cross(                const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2);
D3DXVECTOR3        randUnitVector(       RandomGenerator   &rnd = *RandomGenerator::s_stdGenerator);
D3DXVECTOR3        ortonormalVector(     const D3DXVECTOR3 &v);
String             toString(             const D3DXVECTOR3 &v, int dec = 3);
String             toString(             const D3DXVECTOR4 &v, int dec = 3);
String             toString(             const D3DXQUATERNION &q, int dec = 3);
D3DXMATRIX         transpose(            const D3DXMATRIX  &m);
D3DXMATRIX         inverse(              const D3DXMATRIX  &m);
D3DXMATRIX         createIdentityMatrix();
D3DXMATRIX         createTranslateMatrix(const D3DXVECTOR3 &v);
D3DXVECTOR3        operator*(            const D3DXMATRIX  &m   , const D3DXVECTOR3 &v);
D3DXVECTOR3        operator*(            const D3DXVECTOR3 &v   , const D3DXMATRIX  &m);
// return quarternion that rotates unit-vector[unitIndex] pointing into dir
D3DXQUATERNION     createOrientation(    const D3DXVECTOR3 &dir , int unitIndex=0);
D3DXQUATERNION     createOrientation(    const D3DXVECTOR3 &dir , const D3DXVECTOR3 &up);
// return quarternion that rotates from into to
D3DXQUATERNION     createRotation(       const D3DXVECTOR3 &from, const D3DXVECTOR3 &to);
// angle in radians
D3DXQUATERNION     createRotation(       const D3DXVECTOR3 &axis, float angle);
D3DXVECTOR3        rotate(               const D3DXVECTOR3 &v, const D3DXQUATERNION &q);
inline D3DXVECTOR3 rotate(               const D3DXVECTOR3 &v, const D3DXVECTOR3    &axis, float rad) {
  return rotate(v, createRotation(axis, rad));
}
void               getDirUp(             const D3DXQUATERNION &q, D3DXVECTOR3 &dir, D3DXVECTOR3 &up);
float              det(                  const D3DXMATRIX  &m);
String             toString(             const D3DXMATRIX  &m, int dec = 3);

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
  inline operator Point3D()     const                    { return Point3D(     x, y, z); }                                          \
  inline const D3DXVECTOR3 &getPos() const { return *(D3DXVECTOR3*)(void*)&x; }

#define A_NORMAL_TRAITS(className)                                                                                                  \
  template<typename TX,typename TY,typename TZ> className &setNormal(const TX &_x, const TX &_y, const TX &_z) {                    \
    nx = (float)_x ; ny = (float)_y ; nz = (float)_z; return *this;                                                                 \
  }                                                                                                                                 \
  template<typename TV> className &setNormal(const TV &n) {                                                                         \
    nx = (float)n.x; ny = (float)n.y; nz = (float)n.z; return *this;                                                                \
  }                                                                                                                                 \
  inline className &reverseNormal() { nx = -nx; ny = -ny; nz = -nz; return *this; }                                                 \
  inline const D3DXVECTOR3 &getNormal() const { return *(D3DXVECTOR3*)(void*)&nx; }

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

#define NORMALFVFFLAGS  (D3DFVF_XYZ | D3DFVF_NORMAL)
#define TEXTUREFVFFLAGS (D3DFVF_XYZ | D3DFVF_TEX1)

inline bool hasVertexNormals(DWORD fvf) {
  return (fvf & NORMALFVFFLAGS) == NORMALFVFFLAGS;
}

inline bool hasTextureVertices(DWORD fvf) {
  return (fvf & TEXTUREFVFFLAGS) == TEXTUREFVFFLAGS;
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

  inline VertexNormal() {
  }
  template<typename P, typename N> VertexNormal(const Point3DTemplate<P> &pos, const Point3DTemplate<N> &normal)
    :  x((float)pos.x   ),  y((float)pos.y   ),  z((float)pos.z   )
    , nx((float)normal.x), ny((float)normal.y), nz((float)normal.z)
  {
  }
  inline VertexNormal(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &normal)
    :  x(pos.x   ),  y(pos.y   ),  z(pos.z   )
    , nx(normal.x), ny(normal.y), nz(normal.z)
  {
  }
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

UINT FVFToSize(   DWORD     fvf);
UINT formatToSize(D3DFORMAT f  );

class Line3D {
public:
  Vertex m_p1, m_p2;
  inline Line3D() {
  }
  inline Line3D(const Vertex &p1, const Vertex &p2) : m_p1(p1), m_p2(p2) {
  }
  template<typename P1, typename P2> Line3D(const Point3DTemplate<P1> &p1, const Point3DTemplate<P2> &p2)
    : m_p1(p1), m_p2(p2) {
  }
};

typedef CompactArray<Vertex>        VertexArray;
typedef CompactArray<TextureVertex> TextureVertexArray;

class CurveArray : public Array<VertexArray> {
public:
  static CurveArray createSphereObject(double r);
};

#include "PragmaLib.h"
