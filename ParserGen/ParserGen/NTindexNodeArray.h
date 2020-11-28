#pragma once

#include "NTIndexNode.h"

namespace TransposedSuccessorMatrixCompression {

class NTIndexNodeArray : public CompactArray<const NTIndexNode*> {
private:
  const Grammar &m_grammar;
public:
  NTIndexNodeArray(const Grammar &grammar);
  ~NTIndexNodeArray() override;
  void clear() override;
  String toString() const;
};

}; // namespace TransposedSuccessorMatrixCompression
