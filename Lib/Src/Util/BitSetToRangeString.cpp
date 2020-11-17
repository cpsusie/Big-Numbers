#include "pch.h"
#include <String.h>
#include <BitSet.h>

String BitSet::toRangeString(AbstractStringifier<size_t> &sf, const TCHAR *delim) const {
  return getIterator().toRangeString(sf, delim);
}
