#include "pch.h"

void BigInt::assertIsValid() const {
  __super::assertIsValid();
  if(_isnormal()) {
    if(m_low < 0) {
      throwNotValidException(__TFUNCTION__, _T("m_low = %s"), format1000(getLow()).cstr());
    }
  }
}
