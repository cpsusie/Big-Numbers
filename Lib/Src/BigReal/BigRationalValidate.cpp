#include "pch.h"

void BigRational::assertIsValid(const TCHAR *file, int line, const TCHAR *name) const {
  m_num.assertIsValid(file, line, name);
  m_den.assertIsValid(file, line, name);
  if(!m_num._isfinite()) {
    m_num.throwNotValidException(file, line, name, _T("numerator is not finite"));
  }
  if(!m_den._isfinite()) {
    m_den.throwNotValidException(file, line, name, _T("denominator is not finite"));
  }
  switch(_fpclass(m_den)) {
  case _FPCLASS_PZ:
    if(BigReal::compareAbs(m_num, getDigitPool()->_1()) > 0) {
      throwNotValidException(file,line,name, _T("denominator=0, but numerator not in {-1,0,1}. (=%s)")
                            ,toString(m_num).cstr());
    }
    break;
  case _FPCLASS_PN:
    switch(_fpclass(m_num)) {
    case _FPCLASS_PN:
    case _FPCLASS_NN:
      break;
    default         :
      throwNotValidException(file, line, name, _T("denominator > 0. _fpclass(numerator):%04x")
                            ,_fpclass(m_num));
    }
    break;

  default         :
    throwNotValidException(file, line, name, _T("denominator must be >= 0. _fpclass(denominator):%04x")
                          ,_fpclass(m_den));
  }
}
