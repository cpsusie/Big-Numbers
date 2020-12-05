#pragma once

#include <CompactArray.h>

typedef CompactUShortArray StateArray;

inline int stateArrayCmp(const StateArray &a1, const StateArray &a2) {
  return compactShortArrayCmp((const CompactShortArray&)a1, (const CompactShortArray&)a2);
}
