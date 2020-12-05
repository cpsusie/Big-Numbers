#include "pch.h"
#include <BitSet.h>
#include <AbstractSymbolNameContainer.h>

UINT AbstractSymbolNameContainer::getMaxTermNameLength() const {
  if(m_maxTermNameLength == 0) {
    const UINT n = getTermCount();
    size_t     m = 0;
    for(UINT i = 0; i < n; i++) {
      const size_t l = getSymbolName(i).length();
      if(l > m) {
        m = l;
      }
    }
    m_maxTermNameLength = (UINT)m;
  }
  return m_maxTermNameLength;
}

UINT AbstractSymbolNameContainer::getMaxNTermNameLength() const {
  if(m_maxNTermNameLength == 0) {
    const UINT n = getSymbolCount();
    size_t     m = 0;
    for(UINT i = getTermCount(); i < n; i++) {
      const size_t l = getSymbolName(i).length();
      if(l > m) {
        m = l;
      }
    }
    m_maxNTermNameLength = (UINT)m;
  }
  return m_maxNTermNameLength;
}

UINT AbstractSymbolNameContainer::getMaxSymbolNameLength() const {
  return max(getMaxTermNameLength(), getMaxNTermNameLength());
}

class SymbolStringifier: public AbstractStringifier<size_t> {
private:
  const AbstractSymbolNameContainer &m_nameContainer;
  const UINT                         m_offset;
public:
  inline SymbolStringifier(const AbstractSymbolNameContainer &nameContainer, bool ntIndex = false)
    : m_nameContainer(nameContainer)
    , m_offset(ntIndex ? nameContainer.getTermCount() : 0)
  {
  }
  String toString(const size_t &symbolIndex) override {
    return m_nameContainer.getSymbolName((UINT)symbolIndex + m_offset);
  }
};

String AbstractSymbolNameContainer::symbolSetToString(const BitSet &set) const {
  return set.toString(SymbolStringifier(*this), _T(" "), BT_BRACKETS);
}

String AbstractSymbolNameContainer::NTIndexSetToString(const BitSet &set) const {
  return set.toString(SymbolStringifier(*this, true), _T(" "), BT_BRACKETS);
}
