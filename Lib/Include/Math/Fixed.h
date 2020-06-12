#pragma once

#include "MyUtil.h"         // MFC core and standard components

inline float fixedToFloat(const FIXED &x) {
  return (float)x.value + (float)x.fract / 0x10000u;
}

FIXED floatToFixed(   float x          );
MAT2  getMAT2Rotation(float degree     );
MAT2  getMAT2Id(      float size = 1.0f);
