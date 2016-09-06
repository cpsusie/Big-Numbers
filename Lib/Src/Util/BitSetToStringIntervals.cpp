#include "pch.h"
#include <String.h>
#include <BitSet.h>

#if _BITSET_ATOMSIZE == 32
#define FORMATUINT(v) format(_T("%u"), v)
#elif _BITSET_ATOMSIZE == 64
#define FORMATUINT(v) format(_T("%I64u"), v)
#endif // _BITSET_ATOMSIZE == 64

#define FLUSHRANGE()                                                                \
{ if(delim) result += delim; else delim = _T(",");                                  \
  if(first == last) {                                                               \
    result += FORMATUINT(first);                                                    \
  } else {                                                                          \
    const TCHAR *formatStr = (first + 1 == last) ? _T("%s%s") : _T("%s-%s");        \
    result += format(formatStr, FORMATUINT(first).cstr(), FORMATUINT(last).cstr()); \
  }                                                                                 \
}

String BitSet::toStringIntervals() const {
  String result = _T("(");
  size_t first=1,last=0;
  const TCHAR *delim = NULL;
  for(Iterator<size_t> it = ((BitSet&)*this).getIterator(); it.hasNext();) {
    const size_t v = it.next();
    if(first > last) {
      first = last = v;
    } else if(v == last+1) {
      last = v;
    } else {
      FLUSHRANGE();
      first = last = v;
    }
  }
  if(first <= last) {
    FLUSHRANGE();
  }
  result += _T(")");
  return result;
}

