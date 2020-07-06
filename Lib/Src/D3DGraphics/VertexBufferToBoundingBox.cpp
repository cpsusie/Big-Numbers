#include "pch.h"
#include <D3DGraphics/D3Cube.h>

D3Cube getBoundingBox(LPDIRECT3DVERTEXBUFFER vertexBuffer) {
  const D3DVERTEXBUFFER_DESC desc = getDesc(vertexBuffer);
  if(desc.Size == 0) {
    return D3Cube();
  }
  const UINT itemSize    = FVFToSize(desc.FVF);
  void      *bufferItems = NULL;
  V(vertexBuffer->Lock(0, 0, &bufferItems, D3DLOCK_READONLY));
  const char *vp = (char*)bufferItems, *endp = vp + desc.Size;
  D3DXVECTOR3 pmin = *(Vertex*)vp, pmax = pmin;
  for(vp += itemSize; vp < endp; vp += itemSize) {
    const Vertex &v = *(Vertex*)vp;
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
  V(vertexBuffer->Unlock());
  return D3Cube(pmin,pmax);
}
