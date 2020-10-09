#include "pch.h"
#include "RationalIO.h"

using namespace RationalIO;

istream &operator>>(istream &in, Rational &r) {
  return getRational(in, r);
}

ostream &operator<<(ostream &out, const Rational &r) {
  return putRational(out, r);
}
