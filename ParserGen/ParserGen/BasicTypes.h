#pragma once

#include <CommonHashFunctions.h>
#include <CompactArray.h>
#include <CompactHashMap.h>
#include <AbstractParserTables.h>
#include "Options.h"

// Must match elements in BitSetParameters::s_elementName
typedef enum {
  ETYPE_SYMBOL
 ,ETYPE_TERM
 ,ETYPE_NTINDEX
 ,ETYPE_PRODUCTION
 ,ETYPE_STATE
} ElementType;

class ElementName {
private:
  static const TCHAR *s_elementName[][2];
public:
  static inline const TCHAR *getElementName(ElementType type, bool plur) {
    return s_elementName[type][plur?1:0];
  }
  static String createElementCountText(ElementType type, UINT count);
};

typedef BitSet    SymbolSet;
typedef SymbolSet TermSet;    // capacity always #terminals
typedef BitSet    StateSet;   // capacity always #states
typedef BitSet    NTIndexSet; // capacity always #non-terminals

// Return number of bytes neccessary to have a bitSet ranging from [0..capacity-1]
// if capacity=0, return 0
inline UINT getSizeofBitSet(UINT capacity) {
  return (capacity) ? ((capacity - 1) / 8 + 1) : 0;
}

inline int stringCmp(const String &s1, const String &s2) {
  return _tcscmp(s1.cstr(), s2.cstr());
}

inline int compactShortArrayCmp(const CompactShortArray &a1, const CompactShortArray &a2) {
  const size_t n = a1.size();
  int          c = sizetHashCmp(n, a2.size());
  if(c) return c;
  return n ? memcmp(a1.begin(), a2.begin(), n * sizeof(short)) : 0;
}
