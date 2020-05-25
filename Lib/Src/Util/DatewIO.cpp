#include "pch.h"
#include "DateTimeStreamScanner.h"

using namespace std;

wistream &operator>>(wistream &in, Date &d) {
  return getDate<wistream, wchar_t>(in, d);
}

wostream &operator<<(wostream &out, const Date &d) {
  return putDate(out, d);
}
