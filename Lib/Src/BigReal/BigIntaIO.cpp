#include "pch.h"
#include <Math/BigReal/BigInt.h>
#include "BigRealStream.h"

istream  &operator>>(istream  &in ,       BigInt &x) {
  return getBigInt<istream , char   >(in , x);
}
ostream  &operator<<(ostream  &out, const BigInt &x) {
  return putBigInt(out, x);
}
