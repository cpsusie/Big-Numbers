#include "pch.h"
#include <D3DGraphics/D3TexturePointArray.h>

D3TexturePointArray::D3TexturePointArray(UINT n) : CompactFloatArray(n) {
  const float dx = 1.0f / (n - 1);
  float       x = 0;
  for(UINT j = 0; j < n; j++, x += dx) {
    add(x);
  }
  last() = 1;
}
