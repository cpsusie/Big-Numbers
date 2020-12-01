#include "stdafx.h"
#include "SymbolNode.h"

MixedStatePairArray::MixedStatePairArray(const SymbolNodeBase &base, const StatePairArray &statePairArray)
: SymbolNodeBase(base)
{
  CompactUIntHashMap<UINT, 256>  sameNewStateMap(241); // map from newState -> index into m_statePairBitSetArray
  for(const StatePair sp : statePairArray) {
    const UINT  newState = sp.m_newState;
    const UINT *indexp   = sameNewStateMap.get(newState);
    if(indexp) {
      m_statePairBitSetArray[*indexp].addFromState(sp.m_fromState);
    } else {
      const UINT index = (UINT)m_statePairBitSetArray.size();
      m_statePairBitSetArray.add(StatePairBitSet(getGrammar(), newState, sp.m_fromState, true));
      sameNewStateMap.put(newState, index);
    }
  }
  m_statePairBitSetArray.sortBySetSize();
}

StatePairArray MixedStatePairArray::mergeAll() const {
  StatePairArray result(getFromStateCount());
  result.addAll(m_statePairArray);
  if(!m_statePairBitSetArray.isEmpty()) {
    for(auto it = m_statePairBitSetArray.getIterator(); it.hasNext();) {
      result.addAll((StatePairArray)it.next());
    }
    if(result.size() > 1) {
      result.sortByFromState();
    }
  }
  return result;
}

SymbolNode::SymbolNode(const SymbolNode *parent, const SymbolNodeBase &base, UINT fromStateCount, CompressionMethod compressMethod)
  : SymbolNodeBase(  base                                    )
  , m_parent(        parent                                  )
  , m_fromStateCount(fromStateCount                          )
  , m_recurseLevel(  parent?(parent->getRecurseLevel()+1) : 0)
  , m_compressMethod(compressMethod                          )
{
}

SymbolNode *SymbolNode::allocateSymbolNode(const Grammar &grammar, UINT symbol, const StatePairArray &statePairArray, const MatrixOptimizeParameters &opt) {
  const SymbolNodeBase base(grammar, symbol, opt);
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
  SymbolNode *p                    = new SymbolNodeImmediate(parent, msp, newState, msp.getFromStateCount()); TRACE_NEW(p);
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

SymbolNodeSplit::~SymbolNodeSplit() {
  SAFEDELETE(m_child[0]);
  SAFEDELETE(m_child[1]);
}

SymbolNodeSplit &SymbolNodeSplit::setChild(BYTE index, SymbolNode *child) {
  assert(index < 2);
  assert(m_child[index] == nullptr);
  m_child[index] = child;
  return *this;
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

String SymbolNodeBinSearch::toString() const {
  const String result = __super::toString()  + indentString(m_statePairArray.toString(), 4);
  return indentString(result, m_recurseLevel * 2);
}

String SymbolNodeSplit::toString() const {
  const String cstr0  = indentString(getChild(0).toString(),2);
  const String cstr1  = indentString(getChild(1).toString(),2);
  const String result = __super::toString() + indentString(format(_T("Child 0:\n%s\nChild 1:\n%s"), cstr0.cstr(), cstr1.cstr()), 4);
  return indentString(result, m_recurseLevel * 2);
}

String SymbolNodeImmediate::toString() const {
  const String result = __super::toString() + indentString(m_statePair.toString(), 4);
  return indentString(result, m_recurseLevel * 2);
}

String SymbolNodeBitSet::toString() const {
  const String result = __super::toString() + indentString(m_statePairBitSet.toString(), 4);
  return indentString(result, m_recurseLevel * 2);
}

