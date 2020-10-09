#include "pch.h"
#include "NumberIO.h"

using namespace NumberIO;

wostream &operator<<(wostream &out, const Number &n) {
  return putNumber(out, n);
}

wistream &operator>>(wistream &in, Number &n) {
  return getNumber<wistream, wchar_t>(in, n);
}
