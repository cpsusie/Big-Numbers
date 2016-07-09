#include "stdafx.h"

#pragma warning(disable: 4786)  // name truncated to 255 chars

#include "ExpressionWrapper.h"
#include "MarchingCube/MarchingCube.h"
#include "MarchingCube/IsoMesherMC.h"
#include "MarchingCube/IsoMesherDC.h"
#include "MeshBuilder.h"

using namespace ThreeD;

class FieldFunction : public Isosurface {
private:
  ExpressionWrapper m_exprWrapper;
  float             m_delta;
  bool              m_reverseSign;
  Real             *m_xp,*m_yp,*m_zp,*m_tp;

public:
  FieldFunction(const IsoSurfaceParameters &param);
  void fDensity(float x0, float y0, float z0, float dz, int pointCount, float *densities);
  void fNormal(const D3DXVECTOR3 &point, D3DXVECTOR3 &normal);
};

FieldFunction::FieldFunction(const IsoSurfaceParameters &param) {
  m_exprWrapper.compile(param.m_expr, param.m_machineCode);
  if(!m_exprWrapper.ok()) {
    throwException(_T("%s"), m_exprWrapper.getErrorMessage().cstr());
  }
  m_xp    = m_exprWrapper.getVariableByName("x");
  m_yp    = m_exprWrapper.getVariableByName("y");
  m_zp    = m_exprWrapper.getVariableByName("z");
  m_tp    = m_exprWrapper.getVariableByName("t");
  m_delta = param.m_size / 100;

  m_reverseSign     = false; // dont delete this. Used in fDensity !!
  float sample;
  fDensity(0,0,0,0,1,&sample);
  m_reverseSign     = param.m_originOutside == (sample < 0);
  m_bbox.set(param.m_boundingBox.m_lbn, param.m_boundingBox.m_rtf);
}

void FieldFunction::fDensity(float x0, float y0, float z0, float dz, int pointCount, float *densities) {
  *m_xp = x0; *m_yp = y0; *m_zp = z0;
  if(m_reverseSign) {
    for(int i = 0; i < pointCount; i++, (*m_zp) += dz) {
      densities[i] = -m_exprWrapper.evaluate();
    }
  } else {
    for(int i = 0; i < pointCount; i++, (*m_zp) += dz) {
      densities[i] = m_exprWrapper.evaluate();
    }
  }
}

void FieldFunction::fNormal(const D3DXVECTOR3 &point, D3DXVECTOR3 &normal) {
  *m_xp = point.x; *m_yp = point.y; *m_zp = point.z;
  const double f0 = m_exprWrapper.evaluate();
                   *m_xp += m_delta; normal.x = m_exprWrapper.evaluate() - f0;
  *m_xp = point.x; *m_yp += m_delta; normal.y = m_exprWrapper.evaluate() - f0;
  *m_yp = point.y; *m_zp += m_delta; normal.z = m_exprWrapper.evaluate() - f0;
  normal = m_reverseSign ? -unitVector(normal) : unitVector(normal);
}


LPD3DXMESH createMeshMarchingCube(DIRECT3DDEVICE device, const IsoSurfaceParameters &param) {
  FieldFunction ff(param);
  IsoMesherMC meshCreator(ff);

  const float cellSize = param.m_size;

  meshCreator.setVoxelSize(cellSize, cellSize, cellSize);
  Mesh        *mesh = meshCreator.createMesh();
  if(mesh == NULL) {
    throwException(_T("No polygons generated. Cannot create object"));
  }
//  mesh->computeVertexNormals();
  MeshBuilder mb    = mesh->getMeshBuilder();
  return mb.createMesh(device, param.m_doubleSided);
}
