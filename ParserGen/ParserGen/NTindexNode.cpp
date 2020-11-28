#include "stdafx.h"
#include "NTIndexNode.h"

namespace TransposedSuccessorMatrixCompression {

StateSet StatePairArray::getFromStateSet(UINT stateCount) const {
  StateSet result(stateCount);
  for(const StatePair sp : *this) {
    result.add(sp.m_fromState);
  }
  return result;
}

StateArray StatePairArray::getNewStateArray() const {
  StateArray result(size());
  for(const StatePair sp : *this) {
    result.add(sp.m_newState);
  }
  return result;
}

String StatePairArray::toString() const {
  String result;
  for(const StatePair sp : *this) {
    result += format(_T("   %s\n"), sp.toString().cstr());
  }
  return result;
}

StatePairBitSet::operator StatePairArray() const {
  StatePairArray result(getFromStateCount());
  for(auto it = m_fromStateSet.getIterator(); it.hasNext();) {
    result.add(StatePair((USHORT)it.next(), m_newState));
  }
  return result;
}

MixedStatePairArray::MixedStatePairArray(const NTIndexNodeCommonData &cd, const StatePairArray &statePairArray)
: NTIndexNodeCommonData(cd)
{
  CompactUIntHashMap<UINT, 256>  sameNewStateMap(241); // map from newState -> index into m_statePairBitSetArray
  for(const StatePair sp : statePairArray) {
    const UINT  newState = sp.m_newState;
    const UINT *indexp   = sameNewStateMap.get(newState);
    if(indexp) {
      m_statePairBitSetArray[*indexp].addFromState(sp.m_fromState);
    } else {
      const UINT index = (UINT)m_statePairBitSetArray.size();
      m_statePairBitSetArray.add(StatePairBitSet(newState, sp.m_fromState, m_grammar));
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

NTIndexNode::NTIndexNode(const NTIndexNode *parent, const NTIndexNodeCommonData &cd, UINT fromStateCount, CompressionMethod compressMethod)
  : NTIndexNodeCommonData(cd                                      )
  , m_parent(             parent                                  )
  , m_fromStateCount(     fromStateCount                          )
  , m_recurseLevel(       parent?(parent->getRecurseLevel()+1) : 0)
  , m_compressMethod(     compressMethod                          )
{
}

NTIndexNode *NTIndexNode::allocateNTIndexNode(UINT ntIndex, const Grammar &grammar, const StatePairArray &statePairArray) {
  const Options              &options = Options::getInstance();
  const NTIndexNodeCommonData commonData(ntIndex, grammar);
  if(!options.m_useTableCompression) {
    return new BinSearchNode(nullptr, commonData, statePairArray);
  } else {
    return allocateNode(nullptr, MixedStatePairArray(commonData, statePairArray));
  }
}

NTIndexNode *NTIndexNode::allocateNode(const NTIndexNode *parent, const MixedStatePairArray &msp) {
  const UINT     newStateCountArray  = msp.m_statePairArray.getNewStateCount();
  const UINT     newStateCountBitSet = msp.m_statePairBitSetArray.getNewStateCount();
  const UINT     newStateCount       = newStateCountArray + newStateCountBitSet;
  const Options &options             = Options::getInstance();

  assert(newStateCount >= 1);

  if(newStateCount == 1) {
    if(Options::getInstance().m_pruneTransSuccBitSet) {
      return allocateImmediateDontCareNode(parent, msp);
    }
    if(newStateCountArray == 1) {
      return allocateStatePairArrayNode(parent, msp, msp.m_statePairArray);
    } else { // newStateCountBitSet == 1
      return allocateStatePairBitSetNode(parent, msp, msp.m_statePairBitSetArray.first());
    }
  }

  const BYTE recurseLevel = parent ? parent->getRecurseLevel() + 1 : 0;
  if((newStateCountBitSet == 0) || (msp.m_statePairBitSetArray.first().getFromStateCount() < options.m_minStateBitSetSize) || (recurseLevel >= options.m_maxRecursionTransSucc)) {
    return allocateStatePairArrayNode(parent, msp, msp.mergeAll());
  }
  return allocateSplitNode(parent, msp);
}

// assume (fromStateCount + statePairSetCount >= 1);
NTIndexNode *NTIndexNode::allocateImmediateDontCareNode(const NTIndexNode *parent, const MixedStatePairArray &msp) {
  const UINT   newStateCountArray  = msp.m_statePairArray.getNewStateCount();
  const UINT   newStateCountBitSet = msp.m_statePairBitSetArray.getNewStateCount();
  const UINT   newStateCount       = newStateCountArray + newStateCountBitSet;
  assert(newStateCount == 1);
  const UINT   newState            = (newStateCountArray == 1) ? msp.m_statePairArray.first().m_newState : msp.m_statePairBitSetArray.first().getNewState();
  NTIndexNode *p                   = new ImmediateNode(parent, msp, newState, msp.getFromStateCount()); TRACE_NEW(p);
  return p;
}

NTIndexNode *NTIndexNode::allocateSplitNode(const NTIndexNode *parent, const MixedStatePairArray &msp) {
  const UINT   fromStateCount      = msp.getFromStateCount();
  SplitNode   *p                   = new SplitNode(parent, msp, fromStateCount); TRACE_NEW(p);
  // (newStateCount >= 2) && (newStateCountBitSet >= 1) && (m_statePairBitSetArray.first().getFromStateCount() >= options.m_minStateBitSetSize)
  NTIndexNode *child0 = allocateStatePairBitSetNode(p, msp, msp.m_statePairBitSetArray[0]);
  NTIndexNode *child1 = allocateNode(               p, MixedStatePairArray(msp).removeFirstBitSet());
  p->setChild(0, child0).setChild(1, child1);
  return p;
}

NTIndexNode *NTIndexNode::allocateStatePairArrayNode(const NTIndexNode *parent, const NTIndexNodeCommonData &cd, const StatePairArray &statePairArray) {
  NTIndexNode *p;
  if(statePairArray.getFromStateCount() == 1) {
    p = new ImmediateNode(parent, cd, statePairArray.first()); TRACE_NEW(p);
  } else {
    p = new BinSearchNode(parent, cd, statePairArray        ); TRACE_NEW(p);
  }
  return p;
}

NTIndexNode *NTIndexNode::allocateStatePairBitSetNode(const NTIndexNode *parent, const NTIndexNodeCommonData &cd, const StatePairBitSet &statePairBitSet) {
  NTIndexNode *p;
  if(statePairBitSet.getFromStateCount() == 1) {
    p = new ImmediateNode(parent, cd, StatePairArray(statePairBitSet).first()); TRACE_NEW(p);
  } else {
    p = new BitSetNode(   parent, cd, statePairBitSet                        ); TRACE_NEW(p);
  }
  return p;
}

SplitNode::~SplitNode() {
  SAFEDELETE(m_child[0]);
  SAFEDELETE(m_child[1]);
}

SplitNode &SplitNode::setChild(BYTE index, NTIndexNode *child) {
  assert(index < 2);
  assert(m_child[index] == nullptr);
  m_child[index] = child;
  return *this;
}

String NTIndexNode::toString() const {
  return format(_T("%u - ntIndex %u %s %-20s (From states:%u)\n")
               ,m_recurseLevel
               ,m_ntIndex
               ,getSymbolName().cstr()
               ,compressMethodToString(getCompressionMethod())
               ,getFromStateCount()
               );
}

String BinSearchNode::toString() const {
  const String result = __super::toString()  + indentString(m_statePairArray.toString(), 4);
  return indentString(result, m_recurseLevel * 2);
}

String SplitNode::toString() const {
  const String cstr0  = indentString(getChild(0).toString(),2);
  const String cstr1  = indentString(getChild(1).toString(),2);
  const String result = __super::toString() + indentString(format(_T("Child 0:\n%s\nChild 1:\n%s"), cstr0.cstr(), cstr1.cstr()), 4);
  return indentString(result, m_recurseLevel * 2);
}

String ImmediateNode::toString() const {
  const String result = __super::toString() + indentString(m_statePair.toString(), 4);
  return indentString(result, m_recurseLevel * 2);
}

String BitSetNode::toString() const {
  const String result = __super::toString() + indentString(m_statePairBitSet.toString(), 4);
  return indentString(result, m_recurseLevel * 2);
}

}; // namespace TransposedSuccessorMatrixCompression
