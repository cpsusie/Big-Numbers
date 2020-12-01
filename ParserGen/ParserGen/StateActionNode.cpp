#include "stdafx.h"
#include <CompactHashMap.h>
#include "StateActionNode.h"

namespace ActionMatrixCompression {

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
        m_termSetReductionArray[*indexp].addTerm(pa.m_term);
      } else {
        const UINT index = (UINT)m_termSetReductionArray.size();
        m_termSetReductionArray.add(TermSetReduction(getGrammar(), prod, pa.m_term));
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

StateActionNode *StateActionNode::allocateStateActionNode(const Grammar &grammar, UINT state, const ParserActionArray &actionArray, const MatrixOptimizeParameters &opt) {
  const ActionNodeCommonData commonData(grammar, state, opt);
  const Options             &options = Options::getInstance();
  if(!opt.m_enabled) {
    return new BinSearchNode(nullptr, commonData, actionArray);
  } else {
    return allocateNode(nullptr, ShiftAndReduceActions(commonData, actionArray));
  }
}

StateActionNode *StateActionNode::allocateNode(const StateActionNode *parent, const ShiftAndReduceActions &sra) {
  const UINT                      shiftActions  = sra.m_shiftActionArray.getLegalTermCount();
  const UINT                      reduceActions = (UINT)sra.m_termSetReductionArray.size();
  const MatrixOptimizeParameters &opt           = sra.getOptimizeParam();

  assert(shiftActions + reduceActions >= 1);

  if(shiftActions + reduceActions == 1) {
    if(reduceActions == 0) {
      return allocateBinSearchNode(parent, sra, sra.m_shiftActionArray);
    } else { // reduceActions == 1
      return allocateBitSetNode( parent, sra, sra.m_termSetReductionArray.first());
    }
  }

  const BYTE recurseLevel = parent ? parent->getRecurseLevel() + 1 : 0;
  if((reduceActions == 0) || (sra.m_termSetReductionArray.first().getLegalTermCount() < opt.m_minBitSetSize) || (recurseLevel >= opt.m_maxRecursion)) {
    return allocateBinSearchNode(parent, sra, sra.mergeAll());
  }
  return allocateSplitNode(parent, sra);
}

StateActionNode *StateActionNode::allocateSplitNode(const StateActionNode *parent, const ShiftAndReduceActions &sra) {
  const UINT legalTokenCount = sra.getLegalTermCount();
  SplitNode *p = new SplitNode(parent, sra, legalTokenCount); TRACE_NEW(p);
  // (shiftActions + reduceActions >= 2) && (reduceActions >= 1) && (m_termSetReductionArray.first().getLegalTermCount() >= options.m_minTermBitSetSize)
  StateActionNode *child0 = allocateBitSetNode(p, sra, sra.m_termSetReductionArray.first());
  StateActionNode *child1 = allocateNode(      p, ShiftAndReduceActions(sra).removeFirstTermSet());
  p->setChild(0, child0).setChild(1, child1);
  return p;
}

StateActionNode *StateActionNode::allocateBinSearchNode(const StateActionNode *parent, const ActionNodeCommonData &cd, const ParserActionArray &actionArray) {
  StateActionNode *p;
  if(actionArray.getLegalTermCount() == 1) {
    p = new ImmediateNode(parent, cd, actionArray.first()); TRACE_NEW(p);
  } else {
    p = new BinSearchNode(parent, cd, actionArray        ); TRACE_NEW(p);
  }
  return p;
}

StateActionNode *StateActionNode::allocateBitSetNode(const StateActionNode *parent, const ActionNodeCommonData &cd, const TermSetReduction &termSetReduction) {
  StateActionNode *p;
  if(termSetReduction.getLegalTermCount() == 1) {
    p = new ImmediateNode(parent, cd, ParserActionArray(termSetReduction).first()); TRACE_NEW(p);
  } else {
    p = new BitSetNode(   parent, cd, termSetReduction                           ); TRACE_NEW(p);
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
  return format(_T("State %u %-20s recurseLevel:%u, (Legal tokens:%u)\n"), getState(), compressMethodToString(getCompressionMethod()), m_recurseLevel, m_legalTermCount);
}

String BinSearchNode::toString() const {
  const String result = __super::toString()  + indentString(m_termListActionArray.toString(getGrammar()),3);
  return indentString(result, m_recurseLevel * 2);
}

String SplitNode::toString() const {
  const String cstr0  = indentString(getChild(0).toString(),2);
  const String cstr1  = indentString(getChild(1).toString(),2);
  const String result = __super::toString() + indentString(format(_T("Child 0:\n%s\nChild 1:\n%s"), cstr0.cstr(), cstr1.cstr()), 3);
  return indentString(result, m_recurseLevel * 2);
}

String ImmediateNode::toString() const {
  const String result = __super::toString() + indentString(m_action.toString(getGrammar()),3);
  return indentString(result, m_recurseLevel * 2);
}

String BitSetNode::toString() const {
  const String result = __super::toString() + indentString(m_termSetReduction.toString(),3);
  return indentString(result, m_recurseLevel * 2);
}

}; // namespace ActionMatrixCompression
