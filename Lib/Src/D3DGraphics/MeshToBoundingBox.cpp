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
  D3DXVECTOR3 pmin, pmax;
  bool   firstTime = true;
  for(size_t bp = 0; bp < desc.Size; bp += itemSize) {
    const Vertex &v = *(Vertex*)(((const char*)bufferItems) + bp);
    if(firstTime) {
      pmax = pmin = v;
      firstTime    = false;
    } else {
      if(v.x < pmin.x) {
        pmin.x = v.x;
      } else if(v.x > pmax.x) {
        pmax.x = v.x;
      }
      if(v.y < pmin.y) {
        pmin.y = v.y;
      } else if(v.y > pmax.y) {
        pmax.y = v.y;
      }
      if(v.z < pmin.z) {
        pmin.z = v.z;
      } else if(v.z > pmax.z) {
        pmax.z = v.z;
      }
    }
  }
  V(vertexBuffer->Unlock());
  return D3DXCube3(pmin,pmax);
}
