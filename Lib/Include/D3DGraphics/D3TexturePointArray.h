#pragma once

#include <CompactArray.h>

class D3TexturePointArray : public CompactFloatArray {
public:
  // Generate an array with n equidistant values in interval [0..1]
  // Assume n > 1
  D3TexturePointArray(UINT n);
};
