#include "pch.h"
#include <Math/Fixed.h>

FIXED floatToFixed(float x) {
  FIXED result;
  result.value = (short)floor(x);
  result.fract = (unsigned short)((x - result.value)*0x10000);
  return result;
}

MAT2 getMAT2Rotation(float degree) {
  MAT2 result;
  const float sn = (float)sin(GRAD2RAD(degree));
  const float cs = (float)cos(GRAD2RAD(degree));
  result.eM11 = floatToFixed(cs);
  result.eM12 = floatToFixed(-sn);
  result.eM21 = floatToFixed(sn);
  result.eM22 = result.eM11;
  return result;
}

MAT2 getMAT2Id(float size) {
  MAT2 result;
  result.eM11 = floatToFixed(size);
  result.eM12 = floatToFixed(0);
  result.eM21 = result.eM12;
  result.eM22 = result.eM11;
  return result;
}
