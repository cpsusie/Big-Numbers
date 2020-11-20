#pragma once

#include "StateActionNode.h"

namespace ActionMatrixCompression {

class StateActionNodeArray : public CompactArray<const StateActionNode*> {
private:
  const GrammarTables &m_tables;
public:
  StateActionNodeArray(const GrammarTables &tables);
  ~StateActionNodeArray() override;
  void clear() override;
  String toString() const;
};

}; // namespace ActionMatrixCompression
