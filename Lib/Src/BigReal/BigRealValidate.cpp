#include "pch.h"

static void throwAssertionException(_In_z_ _Printf_format_string_ const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format, argptr);
  va_end(argptr);
  throwBigRealException(_T("assertIsValidBigReal:%s"), msg.cstr());
}

void BigReal::assertIsValidBigReal() const {
  if(m_expo == BIGREAL_NONNORMAL) {
    if(m_first != NULL) {
      throwAssertionException(_T("m_expo == BIGREAL_NONNORMAL, but m_first != NULL"));
    }
    if(m_last != NULL) {
      throwAssertionException(_T("m_expo == BIGREAL_NONNORMAL, but m_last != NULL"));
    }
    switch(m_low) {
    case BIGREAL_ZEROLOW:
      if(m_negative) {
        throwAssertionException(_T("m_negative == true for zero-value"));
      }
      break;
    case BIGREAL_QNANLOW:
      if(m_negative) {
        throwAssertionException(_T("m_negative == true for nan-value"));
      }
      break;
    case BIGREAL_INFLOW:
      // m_negative = true or false....both are valid
      break;
    default:
      throwAssertionException(_T("m_expo == BIGREAL_NONNORMAL, m_low=%zd, valid value for m_low={%d,%d,%d}")
                             ,m_low
                             ,BIGREAL_ZEROLOW, BIGREAL_QNANLOW, BIGREAL_INFLOW);
    }
    return;
  }
  size_t digitCount = 0;
  for(const Digit *p = m_first; p; p = p->next) {
    if(p->n >= BIGREALBASE) {
      throwAssertionException(_T("Digit(%s) (=%s) >= BIGREALBASE (=%s)")
        , format1000(digitCount).cstr()
        , format1000(p->n).cstr()
        , format1000(BIGREALBASE).cstr());
    }
    digitCount++;
  }
  if(digitCount == 0) {
    throwAssertionException(_T("#digits in chain = 0. x != 0"));
  }
  if(digitCount != getLength()) {
    throwAssertionException(_T("#digits in chain (=%s) != getLength() (=%s)")
      , format1000(digitCount).cstr()
      , format1000(getLength()).cstr());
  }
  if(m_first->n == 0) {
    throwAssertionException(_T("m_first->n = 0"));
  }
  if(m_last->n == 0) {
    throwAssertionException(_T("m_last->n = 0"));
  }
  if(m_expo > BIGREAL_MAXEXPO) {
    throwAssertionException(_T("m_expo > BIGREAL_MAXEXPO (=%s)"), format1000(m_expo).cstr());
  }
  if(m_expo < BIGREAL_MINEXPO) {
    throwAssertionException(_T("m_expo < BIGREAL_MINEXPO (=%s)"), format1000(m_expo).cstr());
  }
}
