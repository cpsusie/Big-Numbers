#include "pch.h"
#include "NumberStr.h"

using namespace NumberStr;

Packer &operator<<(Packer &p, const Number &n) {
  switch(n.getType()) {
  case NUMBERTYPE_FLOAT   : p << (float   )n; break;
  case NUMBERTYPE_DOUBLE  : p << (double  )n; break;
  case NUMBERTYPE_DOUBLE80: p << (Double80)n; break;
  case NUMBERTYPE_RATIONAL: p << (Rational)n; break;
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("Unknown type:%d"), n.getType());
  }
  return p;
}

Packer &operator>>(Packer &p, Number &n) {
  switch(p.peekType()) {
  case Packer::E_CHAR     :
  case Packer::E_SHORT    :
  case Packer::E_RESERVED :
  case Packer::E_LONG     :
  case Packer::E_LONG_LONG:
  case Packer::E_FLOAT    :
  case Packer::E_DOUBLE   :
  case Packer::E_DOUBLE80 :
    { Double80 d80;
      p >> d80;
      setNumberFloatValue(n, d80);
    }
    break;
  case Packer::E_RATIONAL :
    { Rational r;
      p >> r;
      n = r;
    }
    break;
  default:
    throwException(_T("%s:Invalid type:%d. Expected E_CHAR/SHORT/LONG/LONG_LONG/FLOAT/DOUBLE/DOUBLE80/RATIONAL"), __TFUNCTION__, p.peekType());
  }
  return p;
}
