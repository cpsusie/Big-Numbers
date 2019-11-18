#include "pch.h"

void BigInt::assertIsValid(const TCHAR *file, int line, const TCHAR *name) const {
  __super::assertIsValid(file, line, name);
  if(_isnormal()) {
    if(m_low < 0) {
      throwNotValidException(file,line,name, _T("m_low = %s"), format1000(getLow()).cstr());
    }
  }
}
