#include "pch.h"
#include "ComplexIO.h"

using namespace ComplexIO;

wistream &operator>>(wistream &in, Complex &c) {
  return getComplex<wistream, wchar_t>(in, c);
}

wostream &operator<<(wostream &out, const Complex &c) {
  return putComplex(out, c);
}
