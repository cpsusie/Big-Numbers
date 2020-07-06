#include "pch.h"
#include <D3DGraphics/D3ToString.h>

D3DVERTEXBUFFER_DESC getDesc(LPDIRECT3DVERTEXBUFFER vertexBuffer) {
  D3DVERTEXBUFFER_DESC desc;
  V(vertexBuffer->GetDesc(&desc));
  return desc;
}