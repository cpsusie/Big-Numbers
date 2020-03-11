#pragma once

#include <MFCUtil/DirectXDeviceFactory.h>
#include <Math/Spherical.h>

class D3Spherical : public FloatSpherical {
public:
  inline D3Spherical() {
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

