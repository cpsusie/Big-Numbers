#pragma once

#include "D3SceneObjectVisual.h"
#include "D3Cube.h"

class D3SceneObjectWithMesh : public D3SceneObjectVisual {
private:
  D3DFILLMODE  m_fillMode;
  D3DSHADEMODE m_shadeMode;
protected:
  LPD3DXMESH m_mesh;
  void createMesh(DWORD faceCount, DWORD vertexCount, DWORD fvf);
  void *lockVertexBuffer();
  void *lockIndexBuffer();
  void unlockVertexBuffer();
  void unlockIndexBuffer();
  void releaseMesh();
  inline void drawSubset(DWORD attribId) {
    V(getMesh()->DrawSubset(attribId));
  }
public:
  // if mesh != NULL, it will be released when Object is deleted
  D3SceneObjectWithMesh(D3Scene             &scene , LPD3DXMESH mesh = NULL, const String &name = _T("MeshObject"));
  D3SceneObjectWithMesh(D3SceneObjectVisual *parent, LPD3DXMESH mesh = NULL, const String &name = _T("MeshObject"));
  ~D3SceneObjectWithMesh()                        override;
  void draw()                                     override;
  LPD3DXMESH getMesh()                      const override {
    return m_mesh;
  }

  bool hasFillMode()                        const override {
    return true;
  }
  void setFillMode(D3DFILLMODE fillMode)          override {
    m_fillMode = fillMode;
  }
  D3DFILLMODE getFillMode()                 const override {
    return m_fillMode;
  }
  bool hasShadeMode()                       const override {
    return true;
  }
  void setShadeMode(D3DSHADEMODE shadeMode)       override {
    m_shadeMode = shadeMode;
  }
  D3DSHADEMODE getShadeMode()               const override {
    return m_shadeMode;
  }
  String toString()                         const override;
  String getInfoString()                    const override;
  D3Cube getBoundingBox()                   const;
};
