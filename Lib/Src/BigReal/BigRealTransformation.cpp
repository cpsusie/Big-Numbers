#include "pch.h"
#include <Math/BigRealTransformation.h>

DEFINECLASSNAME(BigRealIntervalTransformation);

void BigRealIntervalTransformation::checkPrecision(UINT digits) {
  if(digits == 0) {
    throwBigRealException(_T("%s:Precision = 0 not allowed"), s_className);
  }
}

UINT BigRealIntervalTransformation::setPrecision(UINT digits) {
  checkPrecision(digits);
  const UINT oldDigits = m_digits;
  m_digits = digits;
  return oldDigits;
}

DEFINECLASSNAME(BigRealRectangleTransformation);

void BigRealRectangleTransformation::checkPrecision(UINT digits) {
  if(digits == 0) {
    throwBigRealException(_T("%s:Precision = 0 not allowed"), s_className);
  }
}

UINT BigRealRectangleTransformation::setPrecision(UINT digits) {
  checkPrecision(digits);
  const UINT oldDigits = m_digits;
  m_digits = digits;
  return oldDigits;
}

