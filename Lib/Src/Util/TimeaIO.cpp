#include "pch.h"
#include "DateTimeIO.h"

using namespace DateTimeIO;

istream  &operator>>(istream  &in, Time &t) {
  return getTime<istream, char>(in, t);
}

ostream  &operator<<(ostream  &out, const Time &t) {
  return putTime(out, t);
}
