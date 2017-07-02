#pragma once

#include "Isosurface.h"

namespace ThreeD {

class CsgIsosurface : public Isosurface {
protected:
  typedef enum {
    CSG_UNION
   ,CSG_INTERSECTION
   ,CSG_DIFFERENCE
  } CSGMode;

  inline Isosurface *findIsosurface(float x0, float y0, float z0);

  CSGMode m_csgMode;

  std::list<Isosurface *> m_children;

  float *m_densities;
  int    m_maxPoints;
public:

  CsgIsosurface();
  virtual ~CsgIsosurface();

  void setCsgMode(CSGMode csgMode);
  void addChild(Isosurface *child);
  virtual BoundingBox getBoundingBox(const D3DXMATRIX &combinedTrans);
  void fDensity_n(float x0, float y0, float z0
                 ,float dz, int pointCount, float *densities);
  virtual void fDensity(float x0, float y0, float z0
                       ,float dz, int pointCount, float *densities);
  virtual void fNormal(const D3DXVECTOR3 &point, D3DXVECTOR3 &normal);

#ifdef USE_MATERIAL
  virtual const Material &fMaterial(const D3DXVECTOR3 &point, float density);
#endif
};

} // namespace ThreeD
