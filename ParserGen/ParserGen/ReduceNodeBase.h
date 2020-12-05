#pragma once

#include "CompressNodeBase.h"

class ReduceNodeBase : public CompressNodeBase {
private:
  const UINT m_state;
public:
  ReduceNodeBase(const Grammar &grammar, const MatrixOptimizeParameters &opt, UINT state)
    : CompressNodeBase(grammar,opt)
    , m_state(         state      )
  {
  }
  inline UINT getState() const {
    return m_state;
  }
};
