#pragma once

#include "D3Cube.h"
#include "Profile2D.h"

class AbstractMeshFactory;

class Point2DTo3DConverter {
protected:
  char m_rotateAxis;         // ['x','y','z'] - rotationaxis in 3D space
  char m_rotateAxisAlignsTo; // ['x','y']     - axis in 2D space aligned with rotateAxis
  char m_xTo3Dcoord, m_yTo3Dcoord;
public:
  Point2DTo3DConverter(char rotateAxis = 'x', char rotateAxisAlignsTo = 'x');
  void checkIsValid() const; // throws Exception if not valid
  template<typename T> Point3DTemplate<T> convertPoint(const Point2DTemplate<T> &p) const {
    Point3DTemplate<T> result(0, 0, 0);
    switch (m_xTo3Dcoord) {
    case 'x': result.x() = p.x(); break;
    case 'y': result.y() = p.x(); break;
    case 'z': result.z() = p.x(); break;
    }
    switch (m_yTo3Dcoord) {
    case 'x': result.x() = p.y(); break;
    case 'y': result.y() = p.y(); break;
    case 'z': result.z() = p.y(); break;
    }
    return result;
  }
  inline BYTE getRotateAxisIndex() const {
    return m_rotateAxis - 'x';
  }
  inline char getRotateAxis() const {
    return m_rotateAxis;
  }
  inline char getRotateAxisAlignsTo() const {
    return m_rotateAxisAlignsTo;
  }
};

inline bool operator==(const Point2DTo3DConverter &c1, const Point2DTo3DConverter &c2) {
  return (c1.getRotateAxis() == c2.getRotateAxis()) && (c1.getRotateAxisAlignsTo() == c2.getRotateAxisAlignsTo());
}

inline bool operator!=(const Point2DTo3DConverter &c1, const Point2DTo3DConverter &c2) {
  return !(c1 == c2);
}

#define PRROT_ROTATESMOOTH      0x01
#define PRROT_NORMALSMOOTH      0x02
#define PRROT_INVERTNORMALS     0x04
#define PRROT_USECOLOR          0x08

class ProfileRotationParameters {
public:
  static const IntInterval s_legalEdgeCountInterval;
  ProfileRotationParameters();
  ProfileRotationParameters(const Point2DTo3DConverter &converter, float rad = D3DX_PI * 2.0f, UINT edgeCount = 20, BYTE flags = 0, D3DCOLOR color = 0);
  // throws Exception if not valid
  void checkIsValid() const;
  // throws Exception if not valid
  static void validateEdgeCount(UINT edgeCount);
  Point2DTo3DConverter m_converter;
  float                m_rad;
  UINT                 m_edgeCount;
  D3DCOLOR             m_color;
  FLAGTRAITS(ProfileRotationParameters, BYTE, m_flags);
};

       bool operator==(const ProfileRotationParameters &p1, const ProfileRotationParameters &p2);
inline bool operator!=(const ProfileRotationParameters &p1, const ProfileRotationParameters &p2) {
  return !(p1 == p2);
}

class ProfileStretchParameters {
public:
  inline ProfileStretchParameters() {
  }
  D3DXVECTOR3 d;
};

       bool operator==(const ProfileStretchParameters &p1, const ProfileStretchParameters &p2);
inline bool operator!=(const ProfileStretchParameters &p1, const ProfileStretchParameters &p2) {
  return !(p1 == p2);
}

class VertexNormalWithIndex : public VertexNormal {
public:
  int m_pindex, m_nindex;
  inline VertexNormalWithIndex() : m_pindex(-1), m_nindex(-1) {
  }
  inline VertexNormalWithIndex(const VertexNormal &vn) : VertexNormal(vn), m_pindex(-1), m_nindex(-1) {
  }
  inline bool haspindex() const {
    return m_pindex >= 0;
  }
  inline bool hasnindex() const {
    return m_nindex >= 0;
  }
  inline UINT getpindex() const {
    return m_pindex;
  }
  inline UINT getnindex() const {
    return m_nindex;
  }
};

class VertexCurve3D : public CompactArray<VertexNormalWithIndex> {
public:
  explicit VertexCurve3D(size_t capacity = 0) : CompactArray(capacity) {
  }
  VertexCurve3D(const VertexCurve2D &c, const Point2DTo3DConverter &converter);
  VertexCurve3D(const CompactArray<VertexNormal> &src) {
    setCapacity(src.size());
    for(const VertexNormal v : src) {
      add(v);
    }
  }
  VertexCurve3D rotate(const D3DXQUATERNION &rot) const;
};

class VertexProfile3D : public Array<VertexCurve3D> {
public:
  explicit VertexProfile3D(size_t capacity = 0) : Array(capacity) {
  }
  VertexProfile3D(const VertexProfile2D &p, const Point2DTo3DConverter &converter);
  VertexProfile3D rotate(const D3DXQUATERNION &rot) const;
  D3Cube getBoundingBox() const;
};

LPD3DXMESH rotateProfile(AbstractMeshFactory &amf, const Profile2D &profile, const ProfileRotationParameters &param, bool doubleSided);
