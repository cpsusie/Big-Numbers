#include "pch.h"
#include <StrStream.h>
#include <Math/Complex.h>

using namespace OStreamHelper;

String toString(const Complex &c, StreamSize precision, StreamSize width, FormatFlags flags) {
  return (TowstringStream(precision, width, flags) << c).str().c_str();
}
