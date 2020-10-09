#include "pch.h"
#include "DateTimeIO.h"

using namespace DateTimeIO;

wistream &operator>>(wistream &in, Timestamp &t) {
  return getTimestamp<wistream, wchar_t>(in, t);
}

wostream &operator<<(wostream &out, const Timestamp &t) {
  return putTimestamp(out, t);
}
