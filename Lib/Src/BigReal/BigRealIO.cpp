#include "pch.h"
#include "BigRealStream.h"

istream  &operator>>(istream  &in ,       BigReal &x) {
  return getBigReal<istream , char   >(in , x);
}

ostream  &operator<<(ostream  &out, const BigReal &x) {
  return putBigReal(out, x);
}

wistream &operator>>(wistream &in ,       BigReal &x) {
  return getBigReal<wistream, wchar_t>(in , x);
}

wostream &operator<<(wostream &out, const BigReal &x) {
  return putBigReal(out, x);
}

