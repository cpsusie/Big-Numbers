#pragma once

#include <Math/Cube3D.h>
#include "D3Math.h"

class D3Cube : public FloatCube3D {
public:
  inline D3Cube() {
  }
  inline D3Cube(const D3DXVECTOR3 &pmin, const D3DXVECTOR3 &pmax)
    : FloatCube3D(Point3DP(pmin), Point3DP(pmax))
  {
  }
  inline D3Cube(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
    : FloatCube3D(minX, minY, minZ, maxX - minX, maxY - minY, maxZ - minZ)
  {
  }
  template<typename TP, typename TS> D3Cube(const Point3DTemplate<TP> &lbn, const Size3DTemplate<TS> &size)
    : FloatCube3D(lbn, size)
  {
  }
  template<typename TS> D3Cube(const D3DXVECTOR3 &lbn, const Size3DTemplate<TS> &size)
    : FloatCube3D(FloatPoint3D(lbn.x,lbn.y,lbn.z), size)
  {
  }
  inline D3DXVECTOR3 getMin() const {
    return D3DXVECTOR3(Left(), Bottom(), Near());
  }
  inline D3DXVECTOR3 getMax() const {
    return D3DXVECTOR3(Right(),Top(),Far());
  }
  inline D3DXVECTOR3 getCenter() const {
    return Point3DP(center());
  }
  D3Cube operator+(const D3DXVECTOR3 &p) const {
    return D3Cube(getMin() + p, size());
  }
  D3Cube operator-(const D3DXVECTOR3 &p) const {
    return D3Cube(getMin() - p, size());
  }
  // union cube
  D3Cube operator+(const D3Cube &c) const {
    return D3Cube(min(getMinX(),c.getMinX()),min(getMinY(),c.getMinY()),min(getMinZ(),c.getMinZ())
                 ,max(getMaxX(),c.getMaxX()),max(getMaxY(),c.getMaxY()),max(getMaxZ(),c.getMaxZ())
                 );
  }

  static D3Cube getSquareCube(const D3DXVECTOR3 &center, float sideLength) {
    if(sideLength < 0) sideLength = -sideLength;
    const float S05 = sideLength / 2;
    const D3DXVECTOR3 D05(S05, S05, S05); // half diagonal
    return D3Cube(center - D05, center + D05);
  }
  static inline D3Cube getStdCube() {
    return D3Cube(D3DXVECTOR3(-1, -1, -1), D3DXVECTOR3(1,1,1));
  }
};


D3Cube getBoundingBox(LPDIRECT3DVERTEXBUFFER vertexBuffer);
D3Cube getBoundingBox(LPD3DXMESH             mesh        );

template<typename V> D3Cube getBoundingBox(const CompactArray<V> &a) {
  const size_t n = a.size();
  if(n == 0) return D3Cube();
  const V *vp = a.getBuffer(), *endp = vp + n;
  D3DXVECTOR3 pmin = vp->getPos(), pmax = pmin;
  while(++vp < endp) {
    const D3DXVECTOR3 &v = vp->getPos();
    if(v.x < pmin.x) {
      pmin.x = v.x;
    } else if(v.x > pmax.x) {
      pmax.x = v.x;
    }
    if(v.y < pmin.y) {
      pmin.y = v.y;
    } else if(v.y > pmax.y) {
      pmax.y = v.y;
    }
    if(v.z < pmin.z) {
      pmin.z = v.z;
    } else if(v.z > pmax.z) {
      pmax.z = v.z;
    }
  }
  return D3Cube(pmin,pmax);
}
