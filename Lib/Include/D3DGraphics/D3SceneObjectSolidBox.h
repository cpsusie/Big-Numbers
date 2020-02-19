#pragma once

#include "D3SceneObjectWithMesh.h"

class MeshBuilder;

class D3SceneObjectSolidBox : public D3SceneObjectWithMesh {
private:
  int m_materialId;
  void makeSquareFace(MeshBuilder &mb, int v0, int v1, int v2, int v3);
  void init(const Vertex &p1, const Vertex &p2);
public:
  D3SceneObjectSolidBox::D3SceneObjectSolidBox(D3Scene &scene, const D3DXCube3 &cube, int materialId = 0)
    : D3SceneObjectWithMesh(scene)
    , m_materialId(materialId)
  {
    init(cube.getMin(), cube.getMax());
  }

  D3SceneObjectSolidBox::D3SceneObjectSolidBox(D3Scene &scene, const Vertex &p1, const Vertex &p2, int materialId = 0)
    : D3SceneObjectWithMesh(scene)
    , m_materialId(materialId)
  {
    init(p1, p2);
  }
  int getMaterialId() const {
    return m_materialId;
  }
  inline void setmaterialId(int materialId) {
    m_materialId = materialId;
  }
};
