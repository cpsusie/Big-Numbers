#include "pch.h"
#include "Double80IO.h"

using namespace Double80IO;

istream &operator>>(istream &in, Double80 &x) {
  return getDouble80<istream,char>(in, x);
}

ostream &operator<<(ostream &out, const Double80 &x) {
  return putDouble80(out, x);
}
