#include "pch.h"
#include <StrStream.h>
#include <Math/Rational.h>

using namespace OStreamHelper;

String toString(const Rational &r, StreamSize precision, StreamSize width, FormatFlags flags) {
  return (TowstringStream(precision, width, flags) << r).str().c_str();
}
