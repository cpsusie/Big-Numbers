#include "pch.h"
#include <String.h>
#include <BitSet.h>

String BitSet::toRangeString(AbstractStringifier<size_t> &sf, const TCHAR *delim, BracketType bracketType) const {
  return getIterator().toRangeString(sf, delim, bracketType);
}
