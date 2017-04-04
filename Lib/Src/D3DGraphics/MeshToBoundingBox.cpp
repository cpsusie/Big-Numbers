#include "pch.h"
#include <D3DGraphics/MeshBuilder.h>
#include <D3DGraphics/D3Scene.h>

DECLARE_THISFILE;

D3DXCube3 getBoundingBox(LPD3DXMESH mesh) {
  LPDIRECT3DVERTEXBUFFER vertexBuffer;
//  LPDIRECT3DINDEXBUFFER  indexBuffer;

  V(mesh->GetVertexBuffer(&vertexBuffer));
//  V(mesh->GetIndexBuffer( &indexBuffer ));

  D3DVERTEXBUFFER_DESC desc;
  V(vertexBuffer->GetDesc(&desc));
  const int itemSize = FVFToSize(desc.FVF);
  void *bufferItems = NULL;
  V(vertexBuffer->Lock(0, 0, &bufferItems, D3DLOCK_READONLY));
  D3DXCube3 result;
  bool   firstTime = true;
  for(size_t bp = 0; bp < desc.Size; bp += itemSize) {
    const Vertex &v = *(Vertex*)(((const char*)bufferItems) + bp);
    if(firstTime) {
      result.m_rtf = result.m_lbn = v;
      firstTime    = false;
    } else {
      if(v.x < result.m_lbn.x) {
        result.m_lbn.x = v.x;
      } else if(v.x > result.m_rtf.x) {
        result.m_rtf.x = v.x;
      }
      if(v.y < result.m_lbn.y) {
        result.m_lbn.y = v.y;
      } else if(v.y > result.m_rtf.y) {
        result.m_rtf.y = v.y;
      }
      if(v.z < result.m_lbn.z) {
        result.m_lbn.z = v.z;
      } else if(v.z > result.m_rtf.z) {
        result.m_rtf.z = v.z;
      }
    }
  }
  V(vertexBuffer->Unlock());
  return result;
}
