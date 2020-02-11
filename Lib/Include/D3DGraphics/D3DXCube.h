#pragma once

#include <Math/Cube3D.h>
#include "D3Math.h"

class D3DXCube3 : public FloatCube3D {
public:
  inline D3DXCube3() {
  }
  inline D3DXCube3(const D3DXVECTOR3 &pmin, const D3DXVECTOR3 &pmax)
    : FloatCube3D(Point3DP(pmin), Point3DP(pmax))
  {
  }
  inline D3DXCube3(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
    : FloatCube3D(minX, minY, minZ, maxX - minX, maxY - minY, maxZ - minZ)
  {
  }
  template<typename TP, typename TS> D3DXCube3(const Point3DTemplate<TP> &lbn, const Size3DTemplate<TS> &size)
    : FloatCube3D(lbn, size)
  {
  }
  template<typename TS> D3DXCube3(const D3DXVECTOR3 &lbn, const Size3DTemplate<TS> &size)
    : FloatCube3D(FloatPoint3D(lbn.x,lbn.y,lbn.z), size)
  {
  }
  inline D3DXVECTOR3 getMin() const {
    return D3DXVECTOR3(getX(), getY(), getZ());
  }
  inline D3DXVECTOR3 getMax() const {
    return D3DXVECTOR3(getX()+getWidth(), getY()+getHeight(), getZ()+getDepth());
  }
  D3DXCube3 operator+(const D3DXVECTOR3 &p) const {
    return D3DXCube3(getMin() + p, getSize());
  }
  D3DXCube3 operator-(const D3DXVECTOR3 &p) const {
    return D3DXCube3(getMin() - p, getSize());
  }

  static D3DXCube3 getSquareCube(const D3DXVECTOR3 &center, float sideLength) {
    if(sideLength < 0) sideLength = -sideLength;
    const float S05 = sideLength / 2;
    const D3DXVECTOR3 D05(S05, S05, S05); // half diagonal
    return D3DXCube3(center - D05, center + D05);
  }
  static D3DXCube3 getStdCube() {
    return D3DXCube3(-1, -1, -1, 1, 1, 1);
  }

};
