#include "pch.h"
#include <Math/BigInt.h>
#include "BigRealStream.h"

istream  &operator>>(istream  &in ,       BigInt &x) {
  return getBigInt<istream , char   >(in , x);
}
ostream  &operator<<(ostream  &out, const BigInt &x) {
  return putBigInt(out, x);
}
wistream &operator>>(wistream &in ,       BigInt &x) {
  return getBigInt<wistream, wchar_t>(in , x);
}
wostream &operator<<(wostream &out, const BigInt &x) {
  return putBigInt(out, x);
}

