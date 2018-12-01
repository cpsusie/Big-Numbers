#include "pch.h"
#include <Math/Double80.h>

Packer &operator<<(Packer &p, const Double80 &x) {
  if(isInt64(x)) {
    return p << getInt64(x);
  } else if(isFloat(x)) {
    return p << getFloat(x);
  } else if(isDouble(x)) {
    return p << getDouble(x);
  } else {
    return p.addElement(Packer::E_DOUBLE80, &x, sizeof(Double80));
  }
}

Packer &operator>>(Packer &p, Double80 &x) {
  switch(p.peekType()) {
  case Packer::E_CHAR     :
  case Packer::E_SHORT    :
  case Packer::E_RESERVED :
  case Packer::E_LONG     :
  case Packer::E_LONG_LONG:
    { INT64 i;
      p >> i;
      x = i;
    }
    break;
  case Packer::E_FLOAT    :
  case Packer::E_DOUBLE   :
    { double d;
      p >> d;
      x = d;
    }
    break;
  case Packer::E_DOUBLE80:
    p.getElement(Packer::E_DOUBLE80, &x, sizeof(Double80));
    break;
  default:
    throwException(_T("%s:Invalid type:%d. Expected E_CHAR/SHORT/LONG/LONG_LONG/FLOAT/DOUBLE/DOUBLE80"), __TFUNCTION__, p.peekType());
  }
  return p;
}
