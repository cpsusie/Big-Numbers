#include "pch.h"
#include "ComplexIO.h"

using namespace ComplexIO;

istream &operator>>(istream &in, Complex &c) {
  return getComplex<istream, char>(in, c);
}

ostream &operator<<(ostream &out, const Complex &c) {
  return putComplex(out, c);
}
