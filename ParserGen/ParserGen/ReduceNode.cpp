#include "stdafx.h"
#include "MixedReduceArray.h"
#include "ReduceNodeBinSearch.h"
#include "ReduceNodeSplit.h"
#include "ReduceNodeImmediate.h"
#include "ReduceNodeBitSet.h"

ReduceNode *ReduceNode::allocateReduceNode(const Grammar &grammar, UINT state, const TermActionPairArray &termActionArray, const MatrixOptimizeParameters &opt) {
  const ReduceNodeBase base(grammar, opt, state);
  if(!opt.m_enabled) {
    return new ReduceNodeBinSearch(nullptr, base, termActionArray);
  } else {
    return allocateNode(nullptr, MixedReductionArray(base, termActionArray));
  }
}

ReduceNode *ReduceNode::allocateNode(const ReduceNode *parent, const MixedReductionArray &ma) {
  const UINT                     arrayActionCount = ma.m_termActionArray.getLegalTermCount();
  const UINT                     termBitSetCount  = (UINT)ma.m_termSetReductionArray.size();
  const MatrixOptimizeParameters opt              = ma.getOptimizeParam();

  assert(arrayActionCount + termBitSetCount >= 1);

  if(arrayActionCount + termBitSetCount == 1) {
    if(termBitSetCount == 0) {
      return allocateBinSearchNode(parent, ma, ma.m_termActionArray);
    } else { // termBitSetCount == 1
      return allocateBitSetNode(   parent, ma, ma.m_termSetReductionArray.first());
    }
  }
  const BYTE recurseLevel = parent ? parent->getRecurseLevel() + 1 : 0;
  if((termBitSetCount == 0) || (ma.m_termSetReductionArray.first().getLegalTermCount() < opt.m_minBitSetSize) || (recurseLevel >= opt.m_maxRecursion)) {
    return allocateBinSearchNode(parent, ma, ma.mergeAll());
  }
  return allocateSplitNode(parent, ma);
}

ReduceNode *ReduceNode::allocateSplitNode(const ReduceNode *parent, const MixedReductionArray &ma) {
  const UINT legalTokenCount = ma.getLegalTermCount();
  ReduceNodeSplit *p = new ReduceNodeSplit(parent, ma, legalTokenCount); TRACE_NEW(p);
  // (shiftActions + reduceActions >= 2) && (reduceActions >= 1) && (m_termSetReductionArray.first().getLegalTermCount() >= options.m_minTermBitSetSize)
  ReduceNode *child0 = allocateBitSetNode(p, ma, ma.m_termSetReductionArray.first());
  ReduceNode *child1 = allocateNode(      p, MixedReductionArray(ma).removeFirstTermSet());
  p->setChild(0, child0).setChild(1, child1);
  return p;
}

ReduceNode *ReduceNode::allocateBinSearchNode(const ReduceNode *parent, const ReduceNodeBase &base, const TermActionPairArray &termActionArray) {
  ReduceNode *p;
  if(termActionArray.getLegalTermCount() == 1) {
    p = new ReduceNodeImmediate(parent, base, termActionArray.first()); TRACE_NEW(p);
  } else {
    p = new ReduceNodeBinSearch(parent, base, termActionArray        ); TRACE_NEW(p);
  }
  return p;
}

ReduceNode *ReduceNode::allocateBitSetNode(const ReduceNode *parent, const ReduceNodeBase &base, const TermSetReduction &termSetReduction) {
  ReduceNode *p;
  if(termSetReduction.getLegalTermCount() == 1) {
    p = new ReduceNodeImmediate(parent, base, TermActionPairArray(termSetReduction).first()); TRACE_NEW(p);
  } else {
    p = new ReduceNodeBitSet(   parent, base, termSetReduction                             ); TRACE_NEW(p);
  }
  return p;
}

// Call only if getCompressionMethod() == CompCodeBinSearch
const TermActionPairArray &ReduceNode::getTermActionPairArray() const {
  throwUnsupportedOperationException(__TFUNCTION__);
  __assume(0);
  return *new TermActionPairArray();
}

// Call only if getCompressionMethod() == CompCodeSplitNode
const ReduceNode        &ReduceNode::getChild(BYTE index)   const {
  throwUnsupportedOperationException(__TFUNCTION__);
  __assume(0);
  return *this;
}

// Call only if getCompressionMethod() == CompCodeImmediate
const TermActionPair      &ReduceNode::getTermActionPair()     const {
  throwUnsupportedOperationException(__TFUNCTION__);
  __assume(0);
  return *new TermActionPair();
}

// Call only if getCompressionMethod() == CompCodeBitSet
const TermSetReduction  &ReduceNode::getTermSetReduction()  const {
  throwUnsupportedOperationException(__TFUNCTION__);
  __assume(0);
  return *new TermSetReduction(getGrammar(), 0,0);
}

String ReduceNode::toString() const {
  return format(_T("State %u %-20s recurseLevel:%u, (Legal tokens:%u)\n")
               ,getState()
               ,compressMethodToString(getCompressionMethod())
               ,m_recurseLevel
               ,m_legalTermCount
               );
}
