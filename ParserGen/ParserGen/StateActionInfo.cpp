#include "stdafx.h"
#include <CompactHashMap.h>
#include "StateActionInfo.h"

SameReduceActionInfo::operator ActionArray() const {
  ActionArray result(m_termSet.size());
  const short action = -(int)m_prod;
  for(auto it = m_termSet.getIterator(); it.hasNext();) {
    result.add(ParserAction((USHORT)it.next(), action));
  }
  return result;
}

StateActionInfo::StateActionInfo(UINT terminalCount, UINT state, const ActionArray &actionArray)
: m_state(state)
, m_terminalCount(terminalCount)
, m_legalTokenCount((UINT)actionArray.size())
{
  const size_t count = actionArray.size();
  CompactUIntHashMap<UINT, 1999>  sameReductionMap; // map from reduce-production -> index into m_sameReductionArray
  for(auto it = actionArray.getIterator(); it.hasNext();) {
    const ParserAction &pa = it.next();
    if(pa.m_action > 0) {
      m_shiftActionArray.add(pa);
    } else {
      const UINT  prod = -pa.m_action;
      const UINT *indexp = sameReductionMap.get(prod);
      if(indexp) {
        m_sameReductionArray[*indexp].addTerminal(pa.m_token);
      } else {
        const UINT index = (UINT)m_sameReductionArray.size();
        m_sameReductionArray.add(SameReduceActionInfo(terminalCount, prod, pa.m_token));
        sameReductionMap.put(prod, index);
      }
    }
  }
//  m_sameReductionArray.sortBySetSize();
  m_compressMethod = findCompressionMethod();
#if defined(_DEDUG)
  if(getActionArray() != actionArray) {
    throwException(_T("%s:getActionArray failed for state :%u"), __TFUNCTION__, state);
  }
#endif // _DEBUG
}

ActionArray StateActionInfo::getActionArray() const {
  ActionArray result;
  result.addAll(m_shiftActionArray);
  if(m_sameReductionArray.size() > 0) {
    for(auto it = m_sameReductionArray.getIterator(); it.hasNext();) {
      result.addAll((ActionArray)it.next());
    }
    if(result.size() > 1) {
      result.sortByToken();
    }
  }
  return result;
}

CompressionMethod StateActionInfo::findCompressionMethod() {
  const UINT ac = getDifferentActionCount();
  switch(ac) {
  case 1 :
    if(m_shiftActionArray.size() == 1) {
      return ONEITEMCOMPRESSION;
    } else {
      return (m_sameReductionArray[0].getSetSize() == 1)
           ? ONEITEMCOMPRESSION
           : REDUCEBYSAMEPRODCOMPRESSION;
    }
  case 2 :
    return SPLITCOMPRESSION;
  default:
    return UNCOMPRESSED;
  }
}

/*
CompressionMethod StateActionInfo::findCompressionMethod() {
  const UINT ac = getDifferentActionCount();
  if(ac == 1) {
    if(m_shiftActionArray.size() == 1) {
      return ONEITEMCOMPRESSION;
    } else {
      return (m_sameReductionArray[0].getSetSize() == 1)
           ? ONEITEMCOMPRESSION
           : REDUCEBYSAMEPRODCOMPRESSION;
    }
  }

  if(m_sameReductionArray.isEmpty()) {
    return UNCOMPRESSED;
  }
}
*/
