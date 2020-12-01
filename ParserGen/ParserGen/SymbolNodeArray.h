#pragma once

#include "SymbolNode.h"

class StatePairMatrix;

class SymbolNodeArray : public CompactArray<const SymbolNode*> {
private:
  const Grammar &m_grammar;
public:
  SymbolNodeArray(const StatePairMatrix &spm);
  ~SymbolNodeArray() override;
  void clear() override;
  String toString() const;
};
