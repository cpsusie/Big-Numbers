#pragma once

#include "NTindexNode.h"

namespace TransSuccMatrixCompression {

class NTindexNodeArray : public CompactArray<const NTindexNode*> {
private:
  const Grammar &m_grammar;
public:
  NTindexNodeArray(const Grammar &grammar);
  ~NTindexNodeArray() override;
  void clear() override;
  String toString() const;
};

// rows indexed by NTindex, has variable length. Each elment in a row is a StatePair containing m_state,m_newState
// ordered by m_newState, inc
class TransposeSuccessorMatrix : public Array<StatePairArray> {
public:
  TransposeSuccessorMatrix(const Grammar &grammar);
  String toString() const;
};

}; // namespace TransSuccMatrixCompression
