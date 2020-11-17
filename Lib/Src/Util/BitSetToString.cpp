#include "pch.h"
#include <String.h>
#include <BitSet.h>

String BitSet::toString(AbstractStringifier<size_t> &sf, const TCHAR *delim) const {
  return getIterator().toString(sf, delim);
}
