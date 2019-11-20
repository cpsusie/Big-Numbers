#include "pch.h"
#include <BCDArray.h>

// Don't use floating point to save space here. These functions are use
// in Pow2Cache::load/save and will cause deadlock if used
Packer &operator<<(Packer &p, const BigReal &v) {
  if(!isnormal(v)) {
    const float f = getFloat(v); // only non-normal floats can be used here (easy way of handling 0, nan, +/-inf)
    p << f;
  } else if(isInt64(v)) {
    p << getInt64(v);
  } else {
    p << BCDArray(toString((FullFormatBigReal&)v));
  }
  return p;
}

Packer &operator>>(Packer &p, BigReal &v) {
  CHECKISMUTABLE(v);
  switch(p.peekType()) {
  case Packer::E_CHAR     :
  case Packer::E_SHORT    :
  case Packer::E_RESERVED :
  case Packer::E_LONG     :
  case Packer::E_LONG_LONG:
    { INT64 i;
      p >> i;
      v = i;
    }
    break;
  case Packer::E_FLOAT    :
    { float f;
      p >> f; 
      if(isnormal(f)) {
        throwException(_T("%s:E_FLOAT is normal (=%.8le), when reading BigReal"), __TFUNCTION__, f);
      }
      v = f;
    }
    break;
  case Packer::E_ARRAY    :
    { BCDArray a;
      p >> a;
      v = BigReal(a.toString(),v.getDigitPool());
    }
    break;
  default:
    throwException(_T("%s:Invalid type:%d. Expected E_CHAR/SHORT/LONG/LONG_LONG/FLOAT/ARRAY"), __TFUNCTION__, p.peekType());
  }
  return p;
}
