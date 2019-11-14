#include "pch.h"

void BigRational::assertIsValid() const {
  DEFINEMETHODNAME;
  m_numerator.assertIsValid();
  m_denominator.assertIsValid();
  if(!m_numerator._isfinite()) {
    throwNotValidException(method, _T("numerator is not finite"));
  }
  if(!m_denominator._isfinite()) {
    throwNotValidException(method, _T("denominator is not finite"));
  }
  switch(_fpclass(m_denominator)) {
  case _FPCLASS_PZ:
    if(BigReal::compareAbs(m_numerator, getDigitPool()->_1()) > 0) {
      throwNotValidException(method, _T("denominator=0, but numerator not in {-1,0,1}. (=%s)")
                            ,toString(m_numerator).cstr());
    }
    break;
  case _FPCLASS_PN:
    switch(_fpclass(m_numerator)) {
    case _FPCLASS_PN:
    case _FPCLASS_NN:
      break;
    default         :
      throwNotValidException(method, _T("denominator > 0. _fpclass(numerator):%04x")
                            ,_fpclass(m_numerator));
    }
    break;

  default         :
    throwNotValidException(method, _T("denominator must be >= 0. _fpclass(denominator):%04x")
                          ,_fpclass(m_denominator));
  }
}
