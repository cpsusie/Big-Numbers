#include "stdafx.h"
#include <CompactHashMap.h>
#include "StateActionInfo.h"

TermSetReduction::operator ParserActionArray() const {
  ParserActionArray result(getTermSetSize());
  const short action = -(int)m_prod;
  for(auto it = m_termSet.getIterator(); it.hasNext();) {
    result.add(ParserAction((USHORT)it.next(), action));
  }
  return result;
}

ShiftAndReduceActions::ShiftAndReduceActions(const InfoNodeCommonData &cd, const ParserActionArray &actionArray)
  : InfoNodeCommonData(cd)
{
  CompactUIntHashMap<UINT, 256>  termSetReductionMap(241); // map from reduce-production -> index into termSetReductionArray
  for(const ParserAction pa : actionArray) {
    if(pa.m_action > 0) {
      m_shiftActionArray.add(pa);
    } else { // pa.action <= 0.....reduce (or accept)
      const UINT  prod   = -pa.m_action;
      const UINT *indexp = termSetReductionMap.get(prod);
      if(indexp) {
        m_termSetReductionArray[*indexp].addTerminal(pa.m_term);
      } else {
        const UINT index = (UINT)m_termSetReductionArray.size();
        m_termSetReductionArray.add(TermSetReduction(prod, pa.m_term, m_nameContainer));
        termSetReductionMap.put(prod, index);
      }
    }
  }
  m_termSetReductionArray.sortBySetSize();
}

ParserActionArray ShiftAndReduceActions::mergeAll() const {
  ParserActionArray result(getLegalTermCount());
  result.addAll(m_shiftActionArray);
  if(!m_termSetReductionArray.isEmpty()) {
    for(auto it = m_termSetReductionArray.getIterator(); it.hasNext();) {
      result.addAll((ParserActionArray)it.next());
    }
    if(result.size() > 1) {
      result.sortByTerm();
    }
  }
  return result;
}

StateActionInfo *StateActionInfo::allocateStateActionInfo(UINT state, const SymbolNameContainer &nameContainer, const ParserActionArray &actionArray) {
  const InfoNodeCommonData commonData(state, nameContainer);
  const Options           &options = Options::getInstance();
  if(!options.m_useTableCompression) {
    return new TermListNode(nullptr, commonData, actionArray);
  } else {
    return allocateNode(nullptr, ShiftAndReduceActions(commonData, actionArray));
  }
}

StateActionInfo *StateActionInfo::allocateNode(const StateActionInfo *parent, const ShiftAndReduceActions &sra) {
  const Options &options       = Options::getInstance();
  const UINT     shiftActions  = sra.m_shiftActionArray.getLegalTermCount();
  const UINT     reduceActions = (UINT)sra.m_termSetReductionArray.size();

  assert(shiftActions + reduceActions >= 1);

  if(shiftActions + reduceActions == 1) {
    if(reduceActions == 0) {
      return allocateTermListNode(parent, sra, sra.m_shiftActionArray);
    } else { // reduceActions == 1
      return allocateTermSetNode( parent, sra, sra.m_termSetReductionArray[0]);
    }
  }

  const BYTE recurseLevel = parent ? parent->getRecurseLevel() + 1 : 0;
  if((reduceActions == 0) || (sra.m_termSetReductionArray[0].getTermSetSize() < 2) || (recurseLevel >= options.m_maxRecursiveCalls)) {
    return allocateTermListNode(parent, sra, sra.mergeAll());
  }

  return allocateSplitNode(parent, sra);
}

StateActionInfo *StateActionInfo::allocateSplitNode(const StateActionInfo *parent, const ShiftAndReduceActions &sra) {
  const UINT legalTokenCount = sra.getLegalTermCount();
  SplitNode *p = new SplitNode(parent, sra, legalTokenCount); TRACE_NEW(p);
  // (shiftActions + reduceActions >= 2) && (reduceActions >= 1) && (m_termSetReductionArray[0].getTermSetSize() >= 2)
  StateActionInfo *child0 = allocateTermSetNode(p, sra, sra.m_termSetReductionArray[0]);
  StateActionInfo *child1 = allocateNode(       p, ShiftAndReduceActions(sra).removeFirstTermSet());
  p->setChild(0, child0).setChild(1, child1);
  return p;
}

StateActionInfo *StateActionInfo::allocateTermListNode(const StateActionInfo *parent, const InfoNodeCommonData &cd, const ParserActionArray &actionArray) {
  StateActionInfo *p;
  if(actionArray.size() == 1) {
    p = new OneItemNode( parent, cd, actionArray[0]); TRACE_NEW(p);
  } else {
    p = new TermListNode(parent, cd, actionArray   ); TRACE_NEW(p);
  }
  return p;
}


StateActionInfo *StateActionInfo::allocateTermSetNode(const StateActionInfo *parent, const InfoNodeCommonData &cd, const TermSetReduction &termSetReduction) {
  StateActionInfo *p;
  if(termSetReduction.getTermSetSize() == 1) {
    const ParserActionArray paa(termSetReduction);
    p = new OneItemNode(parent, cd, paa[0]          ); TRACE_NEW(p);
  } else {
    p = new TermSetNode(parent, cd, termSetReduction); TRACE_NEW(p);
  }
  return p;
}

SplitNode::~SplitNode() {
  SAFEDELETE(m_child[0]);
  SAFEDELETE(m_child[1]);
}

SplitNode &SplitNode::setChild(BYTE index, StateActionInfo *child) {
  assert(index < 2);
  assert(m_child[index] == nullptr);
  m_child[index] = child;
  return *this;
}

String StateActionInfo::toString() const {
  return format(_T("State %u %-20s recurseLevel:%u, (Legal tokens:%u)\n"), m_state, compressMethodToString(getCompressionMethod()), m_recurseLevel, m_legalTermCount);
}

String TermListNode::toString() const {
  const String result = __super::toString()  + indentString(m_termListActionArray.toString(m_nameContainer),3);
  return indentString(result, m_recurseLevel * 2);
}

String SplitNode::toString() const {
  const String cstr0  = indentString(getChild(0).toString(),2);
  const String cstr1  = indentString(getChild(1).toString(),2);
  const String result = __super::toString() + indentString(format(_T("Child 0:\n%s\nChild 1:\n%s"), cstr0.cstr(), cstr1.cstr()), 3);
  return indentString(result, m_recurseLevel * 2);
}

String OneItemNode::toString() const {
  const String result = __super::toString() + indentString(m_action.toString(m_nameContainer),3);
  return indentString(result, m_recurseLevel * 2);
}

String TermSetNode::toString() const {
  const String result = __super::toString() + indentString(m_termSetReduction.toString(),3);
  return indentString(result, m_recurseLevel * 2);
}
