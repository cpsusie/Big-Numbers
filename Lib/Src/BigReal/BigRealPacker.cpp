#include "pch.h"
#include <BCDArray.h>

Packer &operator<<(Packer &p, const BigReal &v) {
  if(isInt64(v)) {
    p << getInt64(v);
  } else if(isDouble80(v)) {
    p << getDouble80(v);
  } else {
    BigRealStream stream;
    stream << FullFormatBigReal(v);
    p << BCDArray(stream);
  }
  return p;
}

Packer &operator>>(Packer &p, BigReal &v) {
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
  case Packer::E_DOUBLE   :
  case Packer::E_DOUBLE80 :
    { Double80 d;
      p >> d;
      v = d;
    }
    break;

  case Packer::E_ARRAY    :
    { BCDArray a;
      p >> a;
      v = BigReal(a.toString(),v.getDigitPool());
    }
    break;
  default:
    throwException(_T("%s:Invalid type:%d. Expected E_CHAR/SHORT/LONG/LONG_LONG/FLOAT/DOUBLE/DOUBLE80/ARRAY"), __TFUNCTION__, p.peekType());
  }
  return p;
}
