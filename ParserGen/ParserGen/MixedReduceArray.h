#pragma once

#include "ReduceNodeBase.h"
#include "TermSetReductionArray.h"
#include "TermActionPairArray.h"

class MixedReductionArray : public ReduceNodeBase {
public:
  TermActionPairArray   m_termActionArray;
  TermSetReductionArray m_termSetReductionArray;
  MixedReductionArray(const ReduceNodeBase &base, const TermActionPairArray &termActionArray);
  TermActionPairArray   mergeAll()          const;
  inline UINT           getLegalTermCount() const {
    return m_termActionArray.getLegalTermCount() + m_termSetReductionArray.getLegalTermCount();
  }



  MixedReductionArray  &removeFirstTermSet() {
    m_termSetReductionArray.removeIndex(0);
    return *this;
  }
};
