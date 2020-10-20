#include "pch.h"
#include "BigRealStream.h"

istream  &operator>>(istream  &in, BigRational &x) {
  return getBigRational<istream, char   >(in, x);
}
ostream  &operator<<(ostream  &out, const BigRational &x) {
  return putBigRational(out, x);
}
