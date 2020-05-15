#pragma once

#include "D3SceneObjectWithVertexBuffer.h"

class D3SceneObjectVisualNormals : public D3SceneObjectVisual {
private:
  D3SceneObjectVisual *m_normalsObject;
  void createNormalsObject(LPDIRECT3DVERTEXBUFFER vertexBuffer);
  void destroyNormalsObject();
public:
  D3SceneObjectVisualNormals(D3SceneObjectVisual *parent);
  ~D3SceneObjectVisualNormals();

  D3DXMATRIX &getWorld() {
    return getParent()->getWorld();
  }
  bool hasNormals() const {
    return false;
  }
  SceneObjectType getType() const {
    return SOTYPE_NORMALSOBJECT;
  }
  void draw();
};
