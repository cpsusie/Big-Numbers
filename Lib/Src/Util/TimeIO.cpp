#include "pch.h"
#include "DateTimeStreamScanner.h"
#include <Date.h>

using namespace std;

istream  &operator>>(istream  &in, Time &t) {
  return getTime<istream, char>(in, t);
}

ostream  &operator<<(ostream  &out, const Time &t) {
  return putTime(out, t);
}
