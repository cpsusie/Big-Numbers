#include "pch.h"
#include "DateTimeStreamScanner.h"

using namespace std;

istream &operator>>(istream &in, Date &d) {
  return getDate<istream, char>(in, d);
}

ostream &operator<<(ostream &out, const Date &d) {
  return putDate(out, d);
}
