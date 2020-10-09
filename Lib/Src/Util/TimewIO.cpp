#include "pch.h"
#include "DateTimeIO.h"

using namespace DateTimeIO;

wistream &operator>>(wistream &in, Time &t) {
  return getTime<wistream, wchar_t>(in, t);
}

wostream &operator<<(wostream &out, const Time &t) {
  return putTime(out, t);
}
