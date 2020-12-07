#pragma once

#include "MyString.h"
#include "MyAssert.h"

class BitSet;

namespace LRParsing {

class AbstractSymbolNameContainer {
private:
  mutable UINT m_maxTermNameLength, m_maxNTermNameLength;
public:
  AbstractSymbolNameContainer() : m_maxTermNameLength(0), m_maxNTermNameLength(0) {
  }
  virtual UINT          getSymbolCount()                        const = 0;
  virtual UINT          getTermCount()                          const = 0;
  virtual const String &getSymbolName(       UINT symbolIndex)  const = 0;
  inline  UINT          getNTermCount()                         const {
    return getSymbolCount() - getTermCount();
  }
  inline  UINT          NTIndexToSymbolIndex(UINT ntIndex    )  const {
    assert(ntIndex < getNTermCount());
    return ntIndex + getTermCount();
  }
  inline  UINT          NTermToNTIndex(      UINT nterm      )  const {
    assert(isNonTerminal(nterm));
    return nterm - getTermCount();
  }
  inline const String  &getNTIndexName(      UINT ntIndex    )  const {
    return getSymbolName(NTIndexToSymbolIndex(ntIndex));
  }
  inline  bool          isTerminal(          UINT symbolIndex)  const {
    assert(symbolIndex < getSymbolCount());
    return symbolIndex < getTermCount();
  }
  inline bool           isNonTerminal(       UINT symbolIndex)  const {
    assert(symbolIndex < getSymbolCount());
    return symbolIndex >= getTermCount();
  }

  UINT                  getMaxTermNameLength()                  const;
  UINT                  getMaxNTermNameLength()                 const;
  UINT                  getMaxSymbolNameLength()                const;
  // Assume max(set) < getSymbolCount()
  String                symbolSetToString(const BitSet &set)    const;
  // Assume max(set) < getNTermCount, add termCount to each index before adding symbolName
  String                NTIndexSetToString(const BitSet &set)    const;
};

}; // namespace LRParsing
