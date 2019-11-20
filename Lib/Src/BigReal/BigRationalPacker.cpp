#include "pch.h"
#include <BCDArray.h>

Packer &operator<<(Packer &p, const BigRational &r) {
  Rational tmp;
  if(isRational(r, &tmp)) {
    p << tmp;
  } else {
    p << BCDArray(toString(r));
  }
  return p;
}

// Call CHECKISMUTABLE
Packer &operator>>(Packer &p, BigRational &r) {
  CHECKISMUTABLE(r);
  switch(p.peekType()) {
  case Packer::E_ARRAY    :
    { BCDArray a;
      p >> a;
      r = BigRational(a.toString(), r.getDigitPool());
    }
    break;
  default:
    { Rational tmp;
      p >> tmp;
      r = tmp;
    }
    break;
  }
  return p;
}
