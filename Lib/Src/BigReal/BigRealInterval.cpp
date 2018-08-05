#include "pch.h"
#include <Math/BigRealInterval.h>

size_t BigRealInterval::getNeededDecimalDigits(const BigReal &from, const BigReal &to, size_t digits) { // static
  const BigReal length = fabs(to - from);
  const BigReal m      = (compareAbs(from,to) > 0) ? fabs(from) : fabs(to);
  return BigReal::getExpo10(m) - BigReal::getExpo10(length) + digits;
}
