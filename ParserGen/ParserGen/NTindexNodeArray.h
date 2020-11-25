#pragma once

#include "NTindexNode.h"

namespace TransposedSuccessorMatrixCompression {

class NTindexNodeArray : public CompactArray<const NTindexNode*> {
private:
  const Grammar &m_grammar;
public:
  NTindexNodeArray(const Grammar &grammar);
  ~NTindexNodeArray() override;
  void clear() override;
  String toString() const;
};

}; // namespace TransposedSuccessorMatrixCompression
