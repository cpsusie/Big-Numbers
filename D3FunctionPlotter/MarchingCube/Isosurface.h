#pragma once

#include <list>

namespace ThreeD {

class Isosurface {
protected:
  D3DXMATRIX   m_localTrans;
  D3DXMATRIX   m_globalTrans;
  D3DXMATRIX   m_globalTransInv;
  BoundingBox m_bbox;
public:
  Isosurface();
  virtual ~Isosurface();

  void setTransform(const D3DXMATRIX &t);
  void addBoundingBox(const BoundingBox &bbox);
  virtual BoundingBox getBoundingBox(const D3DXMATRIX &combinedTrans);
  virtual void fDensity(float x0, float y0, float z0
                       ,float dz, int pointCount, float *densities) = 0;
  virtual void fNormal(const D3DXVECTOR3 &point, D3DXVECTOR3 &normal) = 0;

#ifdef USE_MATERIAL
  virtual const Material &fMaterial(const D3DXVECTOR3 &point, float density) = 0;
#endif
};

} // namespace ThreeD
