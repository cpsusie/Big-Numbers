#include "pch.h"
#include <MathUtil.h>
#include <StrStream.h>

using namespace std;
// dst must point to memory with at least 26 free char
char *flttoa(char *dst, float x) {
  return strcpy(dst, (TostringStream(numeric_limits<float>::max_digits10, 0, ios::scientific) << x).str().c_str());
}

// dst must point to memory with at least 26 free wchar_t
wchar_t *flttow(wchar_t *dst, float x) {
  return wcscpy(dst, (TowstringStream(numeric_limits<float>::max_digits10, 0, ios::scientific) << x).str().c_str());
}
