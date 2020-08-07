#include "pch.h"
#include <StrStream.h>
#include <Math/Number.h>

using namespace OStreamHelper;

String toString(const Number &n, StreamSize precision, StreamSize width, FormatFlags flags) {
  return (TowstringStream(precision, width, flags) << n).str().c_str();
}

