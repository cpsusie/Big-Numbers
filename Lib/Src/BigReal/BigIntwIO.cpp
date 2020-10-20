#include "pch.h"
#include <Math/BigReal/BigInt.h>
#include "BigRealStream.h"

wistream &operator>>(wistream &in ,       BigInt &x) {
  return getBigInt<wistream, wchar_t>(in , x);
}
wostream &operator<<(wostream &out, const BigInt &x) {
  return putBigInt(out, x);
}

