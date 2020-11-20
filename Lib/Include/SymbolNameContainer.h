#pragma once

#include "MyString.h"

class BitSet;

class SymbolNameContainer {
private:
  mutable UINT m_maxTermNameLength, m_maxNTermNameLength;
public:
  SymbolNameContainer() : m_maxTermNameLength(0), m_maxNTermNameLength(0) {
  }
  virtual UINT          getSymbolCount()                        const = 0;
  virtual UINT          getTermCount()                          const = 0;
  virtual const String &getSymbolName(UINT symbolIndex)         const = 0;
  inline UINT           getNTermCount()                         const {
    return getSymbolCount() - getTermCount();
  }
  UINT                  getMaxTermNameLength()                  const;
  UINT                  getMaxNTermNameLength()                 const;
  UINT                  getMaxSymbolNameLength()                const;
  // Assume max(set) < getSymbolCount()
  String                symbolSetToString(const BitSet &set)    const;
};
