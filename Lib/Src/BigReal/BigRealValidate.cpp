#include "pch.h"

void BigReal::assertIsValid() const {
  DEFINEMETHODNAME;
  if(m_expo == BIGREAL_NONNORMAL) {
    if(m_first != NULL) {
      throwNotValidException(method, _T("m_expo == BIGREAL_NONNORMAL, but m_first != NULL"));
    }
    if(m_last != NULL) {
      throwNotValidException(method, _T("m_expo == BIGREAL_NONNORMAL, but m_last != NULL"));
    }
    switch(m_low) {
    case BIGREAL_ZEROLOW:
      if(isNegative()) {
        throwNotValidException(method, _T("Negative-bit is 1 for zero-value"));
      }
      break;
    case BIGREAL_QNANLOW:
      if(isNegative()) {
        throwNotValidException(method, _T("Negative-bit is 1 for nan-value"));
      }
      break;
    case BIGREAL_INFLOW:
      //dont care about negative bit
      break;
    default:
      throwNotValidException(method, _T("m_expo == BIGREAL_NONNORMAL, m_low=%zd, valid value for m_low={%d,%d,%d}")
                            ,m_low
                            ,BIGREAL_ZEROLOW, BIGREAL_QNANLOW, BIGREAL_INFLOW);
    }
    return;
  }
  size_t digitCount = 0;
  for(const Digit *p = m_first; p; p = p->next) {
    if(p->n >= BIGREALBASE) {
      throwNotValidException(method, _T("Digit(%s) (=%s) >= BIGREALBASE (=%s)")
                            ,format1000(digitCount).cstr()
                            ,format1000(p->n).cstr()
                            ,format1000(BIGREALBASE).cstr());
    }
    digitCount++;
  }
  if(digitCount == 0) {
    throwNotValidException(method, _T("#digits in chain = 0. x != 0"));
  }
  if(digitCount != getLength()) {
    throwNotValidException(method, _T("#digits in chain (=%s) != getLength() (=%s)")
                          ,format1000(digitCount).cstr()
                          ,format1000(getLength()).cstr());
  }
  if(m_first->n == 0) {
    throwNotValidException(method, _T("m_first->n = 0"));
  }
  if(m_last->n == 0) {
    throwNotValidException(method, _T("m_last->n = 0"));
  }
  if(m_expo > BIGREAL_MAXEXPO) {
    throwNotValidException(method, _T("m_expo > BIGREAL_MAXEXPO (=%s)"), format1000(m_expo).cstr());
  }
  if(m_expo < BIGREAL_MINEXPO) {
    throwNotValidException(method, _T("m_expo < BIGREAL_MINEXPO (=%s)"), format1000(m_expo).cstr());
  }
}
