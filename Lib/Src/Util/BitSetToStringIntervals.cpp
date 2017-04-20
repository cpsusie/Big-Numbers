#include "pch.h"
#include <String.h>
#include <BitSet.h>

#define FORMATSIZET(v) format(_T("%zu"), v)

#define FLUSHRANGE()                                                                  \
{ if(delim) result += delim; else delim = _T(",");                                    \
  if(first == last) {                                                                 \
    result += FORMATSIZET(first);                                                     \
  } else {                                                                            \
    const TCHAR *formatStr = (first + 1 == last) ? _T("%s%s") : _T("%s-%s");          \
    result += format(formatStr, FORMATSIZET(first).cstr(), FORMATSIZET(last).cstr()); \
  }                                                                                   \
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

