#include "pch.h"
#include "RationalIO.h"

using namespace RationalIO;

wistream &operator>>(wistream &in, Rational &r) {
  return getRational(in, r);
}

wostream &operator<<(wostream &out, const Rational &r) {
  return putRational(out, r);
}
