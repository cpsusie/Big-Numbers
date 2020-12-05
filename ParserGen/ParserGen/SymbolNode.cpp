#include "stdafx.h"
#include "MixedStatePairArray.h"
#include "SymbolNodeBinSearch.h"
#include "SymbolNodeSplit.h"
#include "SymbolNodeImmediate.h"
#include "SymbolNodeBitSet.h"

SymbolNode *SymbolNode::allocateSymbolNode(const Grammar &grammar, UINT symbol, const StatePairArray &statePairArray, const MatrixOptimizeParameters &opt) {
  const SymbolNodeBase base(grammar, opt, symbol);
  if(!opt.m_enabled) {
    return new SymbolNodeBinSearch(nullptr, base, statePairArray);
  } else {
    return allocateNode(nullptr, MixedStatePairArray(base, statePairArray));
  }
}

SymbolNode *SymbolNode::allocateNode(const SymbolNode *parent, const MixedStatePairArray &msp) {
  const UINT     newStateCountArray  = msp.m_statePairArray.getNewStateCount();
  const UINT     newStateCountBitSet = msp.m_statePairBitSetArray.getNewStateCount();
  const UINT     newStateCount       = newStateCountArray + newStateCountBitSet;
  const MatrixOptimizeParameters opt = msp.getOptimizeParam();
 
  assert(newStateCount >= 1);

  if(newStateCount == 1) {
    if(opt.m_pruneBitSet) {
      return allocateImmediateDontCareNode(parent, msp);
    }
    if(newStateCountArray == 1) {
      return allocateStatePairArrayNode(parent, msp, msp.m_statePairArray);
    } else { // newStateCountBitSet == 1
      return allocateStatePairBitSetNode(parent, msp, msp.m_statePairBitSetArray.first());
    }
  }
  const BYTE recurseLevel = parent ? parent->getRecurseLevel() + 1 : 0;
  if((newStateCountBitSet == 0) || (msp.m_statePairBitSetArray.first().getFromStateCount() < opt.m_minBitSetSize) || (recurseLevel >= opt.m_maxRecursion)) {
    return allocateStatePairArrayNode(parent, msp, msp.mergeAll());
  }
  return allocateSplitNode(parent, msp);
}

// assume (fromStateCount + statePairSetCount >= 1 && newStateCount == 1);
SymbolNode *SymbolNode::allocateImmediateDontCareNode(const SymbolNode *parent, const MixedStatePairArray &msp) {
  const UINT   newStateCountArray  = msp.m_statePairArray.getNewStateCount();
  const UINT   newStateCountBitSet = msp.m_statePairBitSetArray.getNewStateCount();
  const UINT   newStateCount       = newStateCountArray + newStateCountBitSet;
  assert(newStateCount == 1);
  const UINT   newState            = (newStateCountArray == 1) ? msp.m_statePairArray.first().m_newState : msp.m_statePairBitSetArray.first().getNewState();
  const UINT   fromStateCount      = msp.getFromStateCount();
  SymbolNode *p                    = new SymbolNodeImmediate(parent, msp, newState, fromStateCount); TRACE_NEW(p);
  return p;
}

SymbolNode *SymbolNode::allocateSplitNode(const SymbolNode *parent, const MixedStatePairArray &msp) {
  const UINT        fromStateCount      = msp.getFromStateCount();
  SymbolNodeSplit  *p                   = new SymbolNodeSplit(parent, msp, fromStateCount); TRACE_NEW(p);
  // (newStateCount >= 2) && (newStateCountBitSet >= 1) && (m_statePairBitSetArray.first().getFromStateCount() >= opt.m_minBitSetSize)
  SymbolNode *child0 = allocateStatePairBitSetNode(p, msp, msp.m_statePairBitSetArray.first());
  SymbolNode *child1 = allocateNode(               p, MixedStatePairArray(msp).removeFirstBitSet());
  p->setChild(0, child0).setChild(1, child1);
  return p;
}

SymbolNode *SymbolNode::allocateStatePairArrayNode(const SymbolNode *parent, const SymbolNodeBase &base, const StatePairArray &statePairArray) {
  SymbolNode *p;
  if(statePairArray.getFromStateCount() == 1) {
    p = new SymbolNodeImmediate(parent, base, statePairArray.first()); TRACE_NEW(p);
  } else {
    p = new SymbolNodeBinSearch(parent, base, statePairArray        ); TRACE_NEW(p);
  }
  return p;
}

SymbolNode *SymbolNode::allocateStatePairBitSetNode(const SymbolNode *parent, const SymbolNodeBase &base, const StatePairBitSet &statePairBitSet) {
  SymbolNode *p;
  if(statePairBitSet.getFromStateCount() == 1) {
    p = new SymbolNodeImmediate(parent, base, StatePairArray(statePairBitSet).first()); TRACE_NEW(p);
  } else {
    p = new SymbolNodeBitSet(   parent, base, statePairBitSet                        ); TRACE_NEW(p);
  }
  return p;
}

String SymbolNode::toString() const {
  if(isTerminal()) {
    return format(_T("%u - term %u %s %-20s (From states:%u)\n")
                  , m_recurseLevel
                  , getTerm()
                  , getSymbolName().cstr()
                  , compressMethodToString(getCompressionMethod())
                  , getFromStateCount()
    );
  } else {
    return format(_T("%u - ntIndex %u %s %-20s (From states:%u)\n")
                  , m_recurseLevel
                  , getNTIndex()
                  , getSymbolName().cstr()
                  , compressMethodToString(getCompressionMethod())
                  , getFromStateCount()
    );
  }
}

const StatePairArray &SymbolNode::getStatePairArray() const {
  throwUnsupportedOperationException(__TFUNCTION__);
  __assume(0);
  return *new StatePairArray(); // should never come here
}

const SymbolNode &SymbolNode::getChild(BYTE index) const {
  throwUnsupportedOperationException(__TFUNCTION__);
  __assume(0);
  return *this; // should never come here
}

// Call only if getCompressionMethod() == CompCodeImmediate
const StatePair &SymbolNode::getStatePair() const {
  throwUnsupportedOperationException(__TFUNCTION__);
  __assume(0);
  return *new StatePair(); // should never come here
}

bool SymbolNode::isDontCareNode() const {
  throwUnsupportedOperationException(__TFUNCTION__);
  __assume(0);
  return false; // should never come here
}

// Call only if getCompressionMethod() == CompCodeBitSet
const StatePairBitSet &SymbolNode::getStatePairBitSet() const {
  throwUnsupportedOperationException(__TFUNCTION__);
  __assume(0);
  return *new StatePairBitSet(getGrammar(), 0,0, isTerminal()); // should never come here
}
