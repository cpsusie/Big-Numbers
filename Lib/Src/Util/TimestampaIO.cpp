#include "pch.h"
#include "DateTimeIO.h"

using namespace DateTimeIO;

istream &operator>>(istream &in, Timestamp &t) {
  return getTimestamp<istream, char>(in, t);
}

ostream &operator<<(ostream &out, const Timestamp &t) {
  return putTimestamp(out, t);
}
