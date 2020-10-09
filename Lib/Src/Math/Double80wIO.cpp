#include "pch.h"
#include "Double80IO.h"

using namespace Double80IO;

wistream &operator>>(wistream &in, Double80 &x) {
  return getDouble80<wistream,wchar_t>(in, x);
}

wostream &operator<<(wostream &out, const Double80 &x) {
  return putDouble80(out, x);
}
