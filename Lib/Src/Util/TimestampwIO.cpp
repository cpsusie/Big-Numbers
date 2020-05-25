#include "pch.h"
#include "DateTimeStreamScanner.h"

using namespace std;

wistream &operator>>(wistream &in, Timestamp &t) {
  return getTimestamp<wistream, wchar_t>(in, t);
}

wostream &operator<<(wostream &out, const Timestamp &t) {
  return putTimestamp(out, t);
}
