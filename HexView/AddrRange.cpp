#include "stdafx.h"
#include "AddrRange.h"

AddrRange::AddrRange(__int64 a1, __int64 a2) {
  if(a1 <= a2) {
    m_first = a1;
    m_last  = a2;
  } else {
    m_first = a2;
    m_last  = a1;
  }
}

bool AddrRange::contains(__int64 addr) const {
  return isEmpty() ? false : ((m_first <= addr) && (addr < m_last));
}

bool AddrRange::overlap(const AddrRange &range) const {
  if(isEmpty() || range.isEmpty()) {
    return false;
  } else {
    return m_first <= range.m_last && m_last >= range.m_first;
  }
}

bool AddrRange::operator==(const AddrRange &r) const {
  if(isEmpty()) {
    return r.isEmpty();
  } else {
    return m_first == r.m_first && m_last == r.m_last;
  }
}

bool AddrRange::operator!=(const AddrRange &r) const {
  return !(operator==(r));
}

String AddrRange::toString() const {
  return isEmpty() ? _T("empty") : format(_T("[%I64d-%I64d]"), m_first, m_last);
}

