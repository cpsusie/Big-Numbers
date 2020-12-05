#pragma once

#include <CompactArray.h>
#include <AbstractParserTables.h>

typedef CompactArray<Action> ActionArray;

inline int actionArrayCmp(const ActionArray &a1, const ActionArray &a2) {
  const size_t n = a1.size();
  int          c = sizetHashCmp(n, a2.size());
  if(c) return c;
  return n ? memcmp(a1.begin(), a2.begin(), n * sizeof(Action)) : 0;
}
