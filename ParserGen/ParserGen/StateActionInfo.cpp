#include "stdafx.h"
#include <CompactHashMap.h>
#include "StateActionInfo.h"

SameReduceActionInfo::operator ActionArray() const {
  ActionArray result(getSetSize());
  const short action = -(int)m_prod;
  for(auto it = m_termSet.getIterator(); it.hasNext();) {
    result.add(ParserAction((USHORT)it.next(), action));
  }
  return result;
}

StateActionInfo::StateActionInfo(UINT terminalCount, UINT state, const ActionArray &actionArray, const SymbolNameContainer &symbolNames)
: m_state(          state                   )
, m_terminalCount(  terminalCount           )
, m_legalTokenCount((UINT)actionArray.size())
, m_symbolNames(    symbolNames             )
{
  initChildren();
  const size_t count = actionArray.size();
  CompactUIntHashMap<UINT, 256>  sameReductionMap(241); // map from reduce-production -> index into m_sameReductionArray
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
        m_sameReductionArray.add(SameReduceActionInfo(terminalCount, prod, pa.m_token, m_symbolNames));
        sameReductionMap.put(prod, index);
      }
    }
  }
  m_sameReductionArray.sortBySetSize();
  m_compressMethod = findCompressionMethod();
#if defined(_DEDUG)
  if(getActionArray() != actionArray) {
    throwException(_T("%s:getActionArray failed for state :%u"), __TFUNCTION__, state);
  }
#endif // _DEBUG
}

StateActionInfo::StateActionInfo(const StateActionInfo &parent, const SameReduceActionInfo &sameReduceAction)
: m_state(             parent.m_state               )
, m_terminalCount(     parent.m_terminalCount       )
, m_legalTokenCount(   sameReduceAction.getSetSize())
, m_symbolNames(       parent.m_symbolNames         )
{
  initChildren();
  m_sameReductionArray.add(sameReduceAction         );
  m_compressMethod     = (m_legalTokenCount == 1) ? ONEITEMCOMPRESSION : REDUCEBYSAMEPRODCOMPRESSION;
}

StateActionInfo::StateActionInfo(const StateActionInfo &parent, const ActionArray &actionArray)
: m_state(             parent.m_state               )
, m_terminalCount(     parent.m_terminalCount       )
, m_legalTokenCount(   (UINT)actionArray.size()     )
, m_symbolNames(       parent.m_symbolNames         )
, m_shiftActionArray(  actionArray                  )
{
  initChildren();
  m_compressMethod     = (m_legalTokenCount == 1) ? ONEITEMCOMPRESSION : UNCOMPRESSED;
}

StateActionInfo::~StateActionInfo() {
  SAFEDELETE(m_child[0]);
  SAFEDELETE(m_child[1]);
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
  const UINT shiftActions  = (UINT)m_shiftActionArray.size();
  const UINT reduceActions = (UINT)m_sameReductionArray.size();
  if(shiftActions + reduceActions == 1) {
    if(reduceActions == 0) {
      return ONEITEMCOMPRESSION;
    } else { // reduceActions == 1
      return (m_sameReductionArray[0].getSetSize() == 1)
           ? ONEITEMCOMPRESSION
           : REDUCEBYSAMEPRODCOMPRESSION;
    }
  }

  if((reduceActions == 0) || (m_sameReductionArray[0].getSetSize() < 5)) {
    return UNCOMPRESSED;
  }

  // (shiftActions + reduceActions >= 2) && (reduceActions >= 1) && (m_sameReductionArray[0].getSetSize() >= 5)
  m_child[0]   = new StateActionInfo(*this, m_sameReductionArray[0]);
  if(shiftActions > 0) {
    if(reduceActions == 0) {
      m_child[1] = new StateActionInfo(*this, m_shiftActionArray);
    } else {
      ActionArray allTheRest(m_shiftActionArray);
      for(UINT i = 1; i < reduceActions; i++) { // [0] is handled in m_child[0]...so start from 1
        allTheRest.addAll(m_sameReductionArray[i]);
      }
      m_child[1] = new StateActionInfo(*this, allTheRest.sortByToken());
    }
  } else { // No shift actions => at least 2 different reduce actions with each terminal-set, at least one with termsetSize >= 5
    if(reduceActions == 2) {
      m_child[1] = new StateActionInfo(*this, m_sameReductionArray[1]);
    } else {
      ActionArray allTheRest;
      for(UINT i = 1; i < reduceActions; i++) {
        allTheRest.addAll(m_sameReductionArray[i]);
      }
      m_child[1] = new StateActionInfo(*this, allTheRest.sortByToken());
    }
  }
  return SPLITNODECOMPRESSION;
}

String StateActionInfo::toString() const {
  String result;
  result = format(_T("State %u (Legal tokens:%u)\n"), m_state, m_legalTokenCount);
  result += m_shiftActionArray.toString(m_symbolNames, true);
  result += m_sameReductionArray.toString();
  result += format(_T("   Compress method:%s\n"), compressMethodToString(getCompressionMethod()));
  if(getCompressionMethod() == SPLITNODECOMPRESSION) {
    const String cstr0 = getChild(0).toString();
    const String cstr1 = getChild(1).toString();
    result += indentString(format(_T("Child 0:\n%s\nChild 1:\n%s"), indentString(cstr0, 2).cstr(), indentString(cstr1, 2).cstr()), 3);
  }
  return result;
}
