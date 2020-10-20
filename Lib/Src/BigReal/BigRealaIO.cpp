#include "pch.h"
#include "BigRealStream.h"

istream  &operator>>(istream  &in ,       BigReal &x) {
  return getBigReal<istream , char   >(in , x);
}
ostream  &operator<<(ostream  &out, const BigReal &x) {
  return putBigReal(out, x);
}
