#include "pch.h"
#include "BigRealStream.h"

wistream &operator>>(wistream &in ,       BigReal &x) {
  return getBigReal<wistream, wchar_t>(in , x);
}
wostream &operator<<(wostream &out, const BigReal &x) {
  return putBigReal(out, x);
}

