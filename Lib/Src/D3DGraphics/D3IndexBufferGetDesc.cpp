#include "pch.h"
#include <D3DGraphics/D3ToString.h>

D3DINDEXBUFFER_DESC getDesc(LPDIRECT3DINDEXBUFFER indexBuffer) {
  D3DINDEXBUFFER_DESC desc;
  V(indexBuffer->GetDesc(&desc));
  return desc;
}
