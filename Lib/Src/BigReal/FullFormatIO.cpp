#include "pch.h"
#include "BigRealStream.h"

ostream  &operator<<(ostream  &out, const FullFormatBigReal &x) {
  return putFullFormatBigReal(out, x);
}

wostream &operator<<(wostream &out, const FullFormatBigReal &x) {
  return putFullFormatBigReal(out, x);
}
