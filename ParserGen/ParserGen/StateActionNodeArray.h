#pragma once

#include "StateActionNode.h"

namespace ActionMatrixCompression {

class StateActionNodeArray : public CompactArray<const StateActionNode*> {
private:
  const Grammar &m_grammar;
public:
  StateActionNodeArray(const Grammar &grammar, const MatrixOptimizeParameters &opt);
  ~StateActionNodeArray() override;
  void clear() override;
  String toString() const;
};

}; // namespace ActionMatrixCompression
