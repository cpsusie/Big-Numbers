#pragma once

#include <limits.h>

// sign of a float value: returns 0 for positive, 1 for negative
#define fsign(f) ((int)((*(long *)(&f) & 0x80000000L) >> 31))

// sign of a double value: returns 0 for positive, 1 for negative
#define dsign(f) ((int)((*(((long *)(&f)) + 1) & 0x80000000L) >> 31))

//----------------------------------------------------------------------------
// roundFloat
//----------------------------------------------------------------------------

inline float roundFloat(float f)
{
    static double r[2] = { 0.0005, -0.0005 };
    return (
        0.001f * (int)(((double)f + r[fsign(f)]) * 1000.0)
    );
}

inline bool isBetween(const D3DXVECTOR3 &v, const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2) {
  return v.x >= v1.x && v.x <= v2.x 
      && v.y >= v1.y && v.y <= v2.y 
      && v.z >= v1.z && v.z <= v2.z;
}

inline D3DXVECTOR3 abs(const D3DXVECTOR3 &v) {
  return D3DXVECTOR3((float)fabs(v.x), (float)fabs(v.y), (float)fabs(v.z));
}

#define isNullVector(v) (((v).x == 0) && ((v).y == 0) && ((v).z == 0))
