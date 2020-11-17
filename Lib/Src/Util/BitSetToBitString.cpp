#include "pch.h"
#include <BitSet.h>

String BitSet::toBitString() const {
  const Atom *p = m_p;
  String result;
  TCHAR tmp[_BS_BITSINATOM + 1], *delim = nullptr;
  for(const Atom *p = m_p, *endp = p + _BS_ATOMCOUNT(m_capacity); p < endp;) {
    if(delim) result += delim; else delim = _T(":");
    result += strRev(sprintBin(tmp, *(p++)));
  }
  return result;
}
