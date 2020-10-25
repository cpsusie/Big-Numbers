#include "pch.h"
#include <D3DGraphics/IsoSurfaceParameters.h>
#include <D3DGraphics/MeshCreators.h>

class SphereIsoFunction : public FunctionR3R1 {
private:
  Real m_r2;
public:
  SphereIsoFunction(const Real radius) : m_r2(radius*radius) {
  }
  Real operator()(const Point3D &p) {
    return p.length2() - m_r2;
  }
};

LPD3DXMESH createSphereMesh(AbstractMeshFactory &amf, double radius) {
  IsoSurfaceParameters param;
  SphereIsoFunction    f(radius);
  const double bb       = ceil(radius + 1);
  param.m_boundingBox   = Cube3D(Point3D(-bb,-bb,-bb), Point3D( bb, bb, bb));
  param.m_originOutside = false;
  param.m_cellSize      = radius / 5;
  param.m_tetrahedral   = false;
  param.m_doubleSided   = false;
  return createMesh(amf, param, f);
}
