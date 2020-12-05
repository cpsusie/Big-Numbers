#pragma once

class AbstractSymbolNameContainer;

class IndexToSymbolConverter {
  const bool m_rowsIndexedByTerminal;
  const UINT m_startSymbol;
  const UINT m_indexCount; // either number of terminals or non-terminals
public:
  IndexToSymbolConverter(const AbstractSymbolNameContainer &nameContainer, bool rowsIndexedByTerminal);
  inline bool getIndexedByTerminal() const {
    return m_rowsIndexedByTerminal;
  }
  inline UINT getStartSymbol() const {
    return m_startSymbol;
  }
  inline UINT getIndexCount() const {
    return m_indexCount;
  }
  inline UINT indexToSymbol(UINT index) const {
    assert(index < m_indexCount);
    return m_startSymbol + index;
  }
};