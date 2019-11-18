#include "pch.h"

void BigRational::assertIsValid(const TCHAR *file, int line, const TCHAR *name) const {
  m_numerator.assertIsValid(file, line, name);
  m_denominator.assertIsValid(file, line, name);
  if(!m_numerator._isfinite()) {
    m_numerator.throwNotValidException(file, line, name, _T("numerator is not finite"));
  }
  if(!m_denominator._isfinite()) {
    m_denominator.throwNotValidException(file, line, name, _T("denominator is not finite"));
  }
  switch(_fpclass(m_denominator)) {
  case _FPCLASS_PZ:
    if(BigReal::compareAbs(m_numerator, getDigitPool()->_1()) > 0) {
      throwNotValidException(file,line,name, _T("denominator=0, but numerator not in {-1,0,1}. (=%s)")
                            ,toString(m_numerator).cstr());
    }
    break;
  case _FPCLASS_PN:
    switch(_fpclass(m_numerator)) {
    case _FPCLASS_PN:
    case _FPCLASS_NN:
      break;
    default         :
      throwNotValidException(file, line, name, _T("denominator > 0. _fpclass(numerator):%04x")
                            ,_fpclass(m_numerator));
    }
    break;

  default         :
    throwNotValidException(file, line, name, _T("denominator must be >= 0. _fpclass(denominator):%04x")
                          ,_fpclass(m_denominator));
  }
}
