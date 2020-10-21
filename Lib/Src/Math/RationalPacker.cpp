#include "pch.h"
#include <Math/Rational.h>

Packer &operator<<(Packer &p, const Rational &x) {
  if(!isfinite(x)) {
    p << (float)x;
  } else if(isInt64(x)) {
    p << (__int64)x;
  } else {
    p.addElement(Packer::E_RATIONAL, nullptr, 0) << x.getNumerator() << x.getDenominator();
  }
  return p;
}

Packer &operator>>(Packer &p, Rational &x) {
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
    { float f;
      p >> f;
      x = Rational(f);
    }
    break;
  case Packer::E_RATIONAL:
    { INT64 n, d;
      p.getElement(Packer::E_RATIONAL, nullptr, 0) >> n >> d;
      x = Rational(n,d);
    }
    break;

  default:
    throwException(_T("%s:Invalid type:%d. Expected E_CHAR/SHORT/LONG/LONG_LONG/FLOAT/RATIONAL"), __TFUNCTION__, p.peekType());
  }
  return p;
}
