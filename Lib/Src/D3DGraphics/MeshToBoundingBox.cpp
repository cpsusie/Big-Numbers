#include "pch.h"
#include <D3DGraphics/D3Scene.h>

DECLARE_THISFILE;

D3DXCube3 getBoundingBox(LPD3DXMESH mesh) {
  LPDIRECT3DVERTEXBUFFER vertexBuffer;
  V(mesh->GetVertexBuffer(&vertexBuffer));
  return getBoundingBox(vertexBuffer);
}
