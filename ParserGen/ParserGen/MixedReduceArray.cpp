#include "stdafx.h"
#include "MixedReduceArray.h"

MixedReductionArray::MixedReductionArray(const ReduceNodeBase &base, const TermActionPairArray &termActionArray)
  : ReduceNodeBase(base)
{
  CompactUIntHashMap<UINT, 256>  termSetReductionMap(241); // map from reduce-production -> index into termSetReductionArray
  for(const TermActionPair tap : termActionArray) {
    if(tap.isShiftAction()) {
      throwInvalidArgumentException(__TFUNCTION__,_T("Shift actions not allowed here"));
    } else {
      const UINT  prod   = tap.getReduceProduction();
      const UINT *indexp = termSetReductionMap.get(prod);
      const UINT  term   = tap.getTerm();
      if(indexp) {
        m_termSetReductionArray[*indexp].addTerm(term);
      } else {
        const UINT index = (UINT)m_termSetReductionArray.size();
        m_termSetReductionArray.add(TermSetReduction(getGrammar(), prod, term));
        termSetReductionMap.put(prod, index);
      }
    }
  }
  m_termSetReductionArray.sortByLegalTermCount();
}

TermActionPairArray MixedReductionArray::mergeAll() const {
  TermActionPairArray result(getLegalTermCount());
  result.addAll(m_termActionArray);
  if(!m_termSetReductionArray.isEmpty()) {
    for(auto it = m_termSetReductionArray.getIterator(); it.hasNext();) {
      result.addAll((TermActionPairArray)it.next());
    }
    if(result.size() > 1) {
      result.sortByTerm();
    }
  }
  return result;
}

