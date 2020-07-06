#include "pch.h"
#include <D3DGraphics/D3ToString.h>

D3DVERTEXBUFFER_DESC getvDesc(LPD3DXMESH mesh) {
  LPDIRECT3DVERTEXBUFFER vertexBuffer = NULL;
  D3DVERTEXBUFFER_DESC   result;
  try {
    V(mesh->GetVertexBuffer(&vertexBuffer)); TRACE_REFCOUNT(vertexBuffer);
    result = getDesc(vertexBuffer);
    SAFERELEASE(vertexBuffer);
  } catch(...) {
    SAFERELEASE(vertexBuffer);
    throw;
  }
  return result;
}

D3DINDEXBUFFER_DESC getiDesc(LPD3DXMESH mesh) {
  LPDIRECT3DINDEXBUFFER indexBuffer = NULL;
  D3DINDEXBUFFER_DESC   result;
  try {
    V(mesh->GetIndexBuffer(&indexBuffer)); TRACE_REFCOUNT(indexBuffer);
    result = getDesc(indexBuffer);
    SAFERELEASE(indexBuffer);
  } catch(...) {
    SAFERELEASE(indexBuffer);
    throw;
  }
  return result;
}
