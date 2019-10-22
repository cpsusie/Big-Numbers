#include "pch.h"
#include "BigRealStream.h"

istream  &operator>>(istream  &in, BigRational &x) {
  return getBigRational<istream, char   >(in, x);
}
ostream  &operator<<(ostream  &out, const BigRational &x) {
  return putBigRational(out, x);
}

wistream &operator>>(wistream &in, BigRational &x) {
  return getBigRational<wistream, wchar_t>(in, x);
}
wostream &operator<<(wostream &out, const BigRational &x) {
  return putBigRational(out, x);
}

