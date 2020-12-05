#pragma once

#include "SymbolNode.h"
#include "IndexToSymbolConverter.h"

class StatePairMatrix;

class SymbolNodeArray : public CompactArray<const SymbolNode*>, public IndexToSymbolConverter {
private:
  const Grammar &m_grammar;
public:
  SymbolNodeArray(const StatePairMatrix &spm);
  ~SymbolNodeArray() override;
  inline const Grammar &getGrammar() const {
    return m_grammar;
  }
  void clear() override;
  String getElementName(UINT index) const;
  String getNullString(UINT index) const;
  String toString() const;
};
