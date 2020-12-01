#pragma once

#include "ReduceNode.h"

namespace TransposedShiftMatrixCompression {

class ReduceNodeArray : public CompactArray<const ReduceNode*> {
private:
  const Grammar &m_grammar;
public:
  ReduceNodeArray(const Grammar &grammar, const MatrixOptimizeParameters &opt);
  ~ReduceNodeArray() override;
  void clear()       override;
  String toString() const;
};

}; // namespace TransposedShiftMatrixCompression
