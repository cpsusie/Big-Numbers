#include "pch.h"
#include "BigRealStream.h"

wistream &operator>>(wistream &in, BigRational &x) {
  return getBigRational<wistream, wchar_t>(in, x);
}
wostream &operator<<(wostream &out, const BigRational &x) {
  return putBigRational(out, x);
}
