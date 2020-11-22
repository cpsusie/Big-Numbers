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
public:
  inline SymbolStringifier(const AbstractSymbolNameContainer &nameContainer) : m_nameContainer(nameContainer) {
  }
  String toString(const size_t &symbolIndex) override {
    return m_nameContainer.getSymbolName((UINT)symbolIndex);
  }
};

String AbstractSymbolNameContainer::symbolSetToString(const BitSet &set) const {
  return set.toString(SymbolStringifier(*this), _T(" "), BT_BRACKETS);
}
