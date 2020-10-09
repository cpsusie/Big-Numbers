#include "pch.h"
#include "DateTimeIO.h"

using namespace DateTimeIO;

wistream &operator>>(wistream &in, Date &d) {
  return getDate<wistream, wchar_t>(in, d);
}

wostream &operator<<(wostream &out, const Date &d) {
  return putDate(out, d);
}
