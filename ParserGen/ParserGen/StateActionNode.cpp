#include "stdafx.h"
#include <CompactHashMap.h>
#include "StateActionNode.h"

namespace ActionMatrixCompression {

TermSetReduction::operator ParserActionArray() const {
  ParserActionArray result(getLegalTermCount());
  const short action = -(int)m_prod;
  for(auto it = m_termSet.getIterator(); it.hasNext();) {
    result.add(ParserAction((USHORT)it.next(), action));
  }
  return result;
}

ShiftAndReduceActions::ShiftAndReduceActions(const ActionNodeCommonData &cd, const ParserActionArray &actionArray)
  : ActionNodeCommonData(cd)
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
        m_termSetReductionArray.add(TermSetReduction(prod, pa.m_term, m_grammar));
        termSetReductionMap.put(prod, index);
      }
    }
  }
  m_termSetReductionArray.sortByLegalTermCount();
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

StateActionNode *StateActionNode::allocateStateActionNode(UINT state, const Grammar &grammar, const ParserActionArray &actionArray) {
  const ActionNodeCommonData commonData(state, grammar);
  const Options             &options = Options::getInstance();
  if(!options.m_useTableCompression) {
    return new BinSearchNode(nullptr, commonData, actionArray);
  } else {
    return allocateNode(nullptr, ShiftAndReduceActions(commonData, actionArray));
  }
}

StateActionNode *StateActionNode::allocateNode(const StateActionNode *parent, const ShiftAndReduceActions &sra) {
  const Options &options       = Options::getInstance();
  const UINT     shiftActions  = sra.m_shiftActionArray.getLegalTermCount();
  const UINT     reduceActions = (UINT)sra.m_termSetReductionArray.size();

  assert(shiftActions + reduceActions >= 1);

  if(shiftActions + reduceActions == 1) {
    if(reduceActions == 0) {
      return allocateBinSearchNode(parent, sra, sra.m_shiftActionArray);
    } else { // reduceActions == 1
      return allocateBitSetNode( parent, sra, sra.m_termSetReductionArray[0]);
    }
  }

  const BYTE recurseLevel = parent ? parent->getRecurseLevel() + 1 : 0;
  if((reduceActions == 0) || (sra.m_termSetReductionArray[0].getLegalTermCount() < 2) || (recurseLevel >= options.m_maxRecursionAction)) {
    return allocateBinSearchNode(parent, sra, sra.mergeAll());
  }

  return allocateSplitNode(parent, sra);
}

StateActionNode *StateActionNode::allocateSplitNode(const StateActionNode *parent, const ShiftAndReduceActions &sra) {
  const UINT legalTokenCount = sra.getLegalTermCount();
  SplitNode *p = new SplitNode(parent, sra, legalTokenCount); TRACE_NEW(p);
  // (shiftActions + reduceActions >= 2) && (reduceActions >= 1) && (m_termSetReductionArray[0].getTermSetSize() >= 2)
  StateActionNode *child0 = allocateBitSetNode(p, sra, sra.m_termSetReductionArray[0]);
  StateActionNode *child1 = allocateNode(       p, ShiftAndReduceActions(sra).removeFirstTermSet());
  p->setChild(0, child0).setChild(1, child1);
  return p;
}

StateActionNode *StateActionNode::allocateBinSearchNode(const StateActionNode *parent, const ActionNodeCommonData &cd, const ParserActionArray &actionArray) {
  StateActionNode *p;
  if(actionArray.getLegalTermCount() == 1) {
    p = new ImmediateNode(parent, cd, actionArray[0]  ); TRACE_NEW(p);
  } else {
    p = new BinSearchNode(parent, cd, actionArray     ); TRACE_NEW(p);
  }
  return p;
}


StateActionNode *StateActionNode::allocateBitSetNode(const StateActionNode *parent, const ActionNodeCommonData &cd, const TermSetReduction &termSetReduction) {
  StateActionNode *p;
  if(termSetReduction.getLegalTermCount() == 1) {
    const ParserActionArray paa(termSetReduction);
    p = new ImmediateNode(parent, cd, paa[0]          ); TRACE_NEW(p);
  } else {
    p = new BitSetNode(   parent, cd, termSetReduction); TRACE_NEW(p);
  }
  return p;
}

SplitNode::~SplitNode() {
  SAFEDELETE(m_child[0]);
  SAFEDELETE(m_child[1]);
}

SplitNode &SplitNode::setChild(BYTE index, StateActionNode *child) {
  assert(index < 2);
  assert(m_child[index] == nullptr);
  m_child[index] = child;
  return *this;
}

String StateActionNode::toString() const {
  return format(_T("State %u %-20s recurseLevel:%u, (Legal tokens:%u)\n"), m_state, compressMethodToString(getCompressionMethod()), m_recurseLevel, m_legalTermCount);
}

String BinSearchNode::toString() const {
  const String result = __super::toString()  + indentString(m_termListActionArray.toString(m_grammar),3);
  return indentString(result, m_recurseLevel * 2);
}

String SplitNode::toString() const {
  const String cstr0  = indentString(getChild(0).toString(),2);
  const String cstr1  = indentString(getChild(1).toString(),2);
  const String result = __super::toString() + indentString(format(_T("Child 0:\n%s\nChild 1:\n%s"), cstr0.cstr(), cstr1.cstr()), 3);
  return indentString(result, m_recurseLevel * 2);
}

String ImmediateNode::toString() const {
  const String result = __super::toString() + indentString(m_action.toString(m_grammar),3);
  return indentString(result, m_recurseLevel * 2);
}

String BitSetNode::toString() const {
  const String result = __super::toString() + indentString(m_termSetReduction.toString(),3);
  return indentString(result, m_recurseLevel * 2);
}

}; // namespace ActionMatrixCompression
