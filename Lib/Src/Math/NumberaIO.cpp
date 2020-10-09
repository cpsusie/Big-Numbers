#include "pch.h"
#include "NumberIO.h"

using namespace NumberIO;

ostream  &operator<<(ostream  &out, const Number &n) {
  return putNumber(out, n);
}

istream  &operator>>(istream  &in, Number &n) {
  return getNumber<istream, char>(in, n);
}
