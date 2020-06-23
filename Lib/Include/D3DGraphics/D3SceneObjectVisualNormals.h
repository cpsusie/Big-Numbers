#pragma once

#include "D3SceneObjectWithVertexBuffer.h"

class D3SceneObjectVisualNormals : public D3SceneObjectVisual {
private:
  D3SceneObjectVisual *m_normalsObject;
  void createNormalsObject(LPDIRECT3DVERTEXBUFFER vertexBuffer);
  void destroyNormalsObject();
public:
  D3SceneObjectVisualNormals(D3SceneObjectVisual *parent);
  ~D3SceneObjectVisualNormals() override;

  SceneObjectType getType() const override {
    return SOTYPE_NORMALSOBJECT;
  }
  D3DXMATRIX &getWorld() override {
    return getParent()->getWorld();
  }
  bool hasNormals() const override {
    return false;
  }
  void draw() override;
};
