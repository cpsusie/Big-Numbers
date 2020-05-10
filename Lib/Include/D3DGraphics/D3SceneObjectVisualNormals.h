#pragma once

#include "D3SceneObjectWithVertexBuffer.h"

class D3SceneObjectVisualNormals : public D3SceneObjectWithVertexBuffer {
private:
  UINT m_normalCount;
  void addArrowVertices(VertexNormal *dst, const VertexNormal &src) const;
  void createNormalsObject(LPDIRECT3DVERTEXBUFFER vertexBuffer);
public:
  D3SceneObjectVisualNormals(D3SceneObjectVisual *parent);

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
