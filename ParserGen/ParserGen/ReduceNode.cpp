#include "stdafx.h"
#include <CompactHashMap.h>
#include "ReduceNode.h"

namespace TransposedShiftMatrixCompression {

MixedReductionArray::MixedReductionArray(const ReduceNodeCommonData &cd, const ParserActionArray &actionArray)
  : ReduceNodeCommonData(cd)
{
  CompactUIntHashMap<UINT, 256>  termSetReductionMap(241); // map from reduce-production -> index into termSetReductionArray
  for(const ParserAction pa : actionArray) {
    if(pa.m_action > 0) {
      ; // do NOTHING
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

ParserActionArray MixedReductionArray::mergeAll() const {
  ParserActionArray result(getLegalTermCount());
  result.addAll(m_parserActionArray);
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

ReduceNode *ReduceNode::allocateReduceNode(const Grammar &grammar, UINT state, const ParserActionArray &actionArray, const MatrixOptimizeParameters &opt) {
  const ReduceNodeCommonData commonData(grammar, state, opt);
  if(!opt.m_enabled) {
    return new ReduceNodeBinSearch(nullptr, commonData, actionArray);
  } else {
    const MixedReductionArray mr(commonData, actionArray);
    return (mr.getLegalTermCount() == 0) ? nullptr : allocateNode(nullptr, mr);
  }
}

ReduceNode *ReduceNode::allocateNode(const ReduceNode *parent, const MixedReductionArray &sra) {
  const UINT                     parserActions = sra.m_parserActionArray.getLegalTermCount();
  const UINT                     reduceActions = (UINT)sra.m_termSetReductionArray.size();
  const MatrixOptimizeParameters opt = sra.getOptimizeParam();

  assert(parserActions + reduceActions >= 1);

  if(parserActions + reduceActions == 1) {
    if(reduceActions == 0) {
      return allocateBinSearchNode(parent, sra, sra.m_parserActionArray);
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

ReduceNode *ReduceNode::allocateSplitNode(const ReduceNode *parent, const MixedReductionArray &sra) {
  const UINT legalTokenCount = sra.getLegalTermCount();
  ReduceNodeSplit *p = new ReduceNodeSplit(parent, sra, legalTokenCount); TRACE_NEW(p);
  // (shiftActions + reduceActions >= 2) && (reduceActions >= 1) && (m_termSetReductionArray.first().getLegalTermCount() >= options.m_minTermBitSetSize)
  ReduceNode *child0 = allocateBitSetNode(p, sra, sra.m_termSetReductionArray.first());
  ReduceNode *child1 = allocateNode(      p, MixedReductionArray(sra).removeFirstTermSet());
  p->setChild(0, child0).setChild(1, child1);
  return p;
}

ReduceNode *ReduceNode::allocateBinSearchNode(const ReduceNode *parent, const ReduceNodeCommonData &cd, const ParserActionArray &actionArray) {
  ReduceNode *p;
  if(actionArray.getLegalTermCount() == 1) {
    p = new ReduceNodeImmediate(parent, cd, actionArray.first()); TRACE_NEW(p);
  } else {
    p = new ReduceNodeBinSearch(parent, cd, actionArray        ); TRACE_NEW(p);
  }
  return p;
}


ReduceNode *ReduceNode::allocateBitSetNode(const ReduceNode *parent, const ReduceNodeCommonData &cd, const TermSetReduction &termSetReduction) {
  ReduceNode *p;
  if(termSetReduction.getLegalTermCount() == 1) {
    p = new ReduceNodeImmediate(parent, cd, ParserActionArray(termSetReduction).first()); TRACE_NEW(p);
  } else {
    p = new ReduceNodeBitSet(   parent, cd, termSetReduction                           ); TRACE_NEW(p);
  }
  return p;
}

ReduceNodeSplit::~ReduceNodeSplit() {
  SAFEDELETE(m_child[0]);
  SAFEDELETE(m_child[1]);
}

ReduceNodeSplit &ReduceNodeSplit::setChild(BYTE index, ReduceNode *child) {
  assert(index < 2);
  assert(m_child[index] == nullptr);
  m_child[index] = child;
  return *this;
}

String ReduceNode::toString() const {
  return format(_T("State %u %-20s recurseLevel:%u, (Legal tokens:%u)\n"), getState(), compressMethodToString(getCompressionMethod()), m_recurseLevel, m_legalTermCount);
}

String ReduceNodeBinSearch::toString() const {
  const String result = __super::toString()  + indentString(m_parserActionArray.toString(getGrammar()),3);
  return indentString(result, m_recurseLevel * 2);
}

String ReduceNodeSplit::toString() const {
  const String cstr0  = indentString(getChild(0).toString(),2);
  const String cstr1  = indentString(getChild(1).toString(),2);
  const String result = __super::toString() + indentString(format(_T("Child 0:\n%s\nChild 1:\n%s"), cstr0.cstr(), cstr1.cstr()), 3);
  return indentString(result, m_recurseLevel * 2);
}

String ReduceNodeImmediate::toString() const {
  const String result = __super::toString() + indentString(m_action.toString(getGrammar()),3);
  return indentString(result, m_recurseLevel * 2);
}

String ReduceNodeBitSet::toString() const {
  const String result = __super::toString() + indentString(m_termSetReduction.toString(),3);
  return indentString(result, m_recurseLevel * 2);
}

}; // namespace TransposedShiftMatrixCompression
