#include "pch.h"
#include <MathUtil.h>
#include <StrStream.h>

using namespace std;
using namespace OStreamHelper;

// dst must point to memory with at least 26 free char
char *dbltoa(char *dst, double x) {
  return strcpy(dst, (TostringStream(numeric_limits<double>::max_digits10, 0, ios::scientific) << x).str().c_str());
}

// dst must point to memory with at least 26 free wchar_t
wchar_t *dbltow(wchar_t *dst, double x) {
  return wcscpy(dst, (TowstringStream(numeric_limits<double>::max_digits10, 0, ios::scientific) << x).str().c_str());
}
