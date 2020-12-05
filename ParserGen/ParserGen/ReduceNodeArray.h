#pragma once

#include "ReduceNode.h"

class ReduceNodeArray : public CompactArray<const ReduceNode*> {
private:
  const Grammar &m_grammar;
public:
  ReduceNodeArray(const Grammar &grammar, const MatrixOptimizeParameters &opt);
  ~ReduceNodeArray() override;
  inline const Grammar &getGrammar() const {
    return m_grammar;
  }
  void clear()       override;
  String getNullString(UINT index) const;
  String toString() const;
};
