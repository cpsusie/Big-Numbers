#include "pch.h"

Packer &operator<<(Packer &p, const BigInt &v) {
  return p << (BigReal&)v;
}

// Call CHECKISMUTABLE
Packer &operator>>(Packer &p, BigInt &v) {
  CHECKISMUTABLE(v);
  BigReal tmp(v.getDigitPool());
  p >> tmp;
  if(!tmp._isnormal() || tmp._isinteger()) {
    v = BigInt(tmp);
  } else {
    throwException(_T("%s:Value is not a BigInt"));
  }
  return p;
}
