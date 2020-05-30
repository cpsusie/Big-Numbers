#include "pch.h"
#include "DateTimeStreamScanner.h"
#include <Date.h>

using namespace std;

wistream &operator>>(wistream &in, Time &t) {
  return getTime<wistream, wchar_t>(in, t);
}

wostream &operator<<(wostream &out, const Time &t) {
  return putTime(out, t);
}
