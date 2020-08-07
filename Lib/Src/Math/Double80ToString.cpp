#include "pch.h"
#include <StrStream.h>
#include <Math/Double80.h>

using namespace OStreamHelper;

String toString(const Double80 &x, StreamSize precision, StreamSize width, FormatFlags flags) {
  return (TowstringStream(precision, width, flags) << x).str().c_str();
}
