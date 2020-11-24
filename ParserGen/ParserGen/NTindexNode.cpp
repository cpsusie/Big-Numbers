#include "stdafx.h"
#include "NTindexNode.h"

namespace TransSuccMatrixCompression {

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

MixedSuccessorTable::MixedSuccessorTable(const NTindexNodeCommonData &cd, const StatePairArray &statePairArray)
: NTindexNodeCommonData(cd)
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

StatePairArray MixedSuccessorTable::mergeAll() const {
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

NTindexNode::NTindexNode(const NTindexNode *parent, const NTindexNodeCommonData &cd, UINT fromStateCount, CompressionMethod compressMethod)
  : NTindexNodeCommonData(cd                                      )
  , m_parent(             parent                                  )
  , m_fromStateCount(     fromStateCount                          )
  , m_recurseLevel(       parent?(parent->getRecurseLevel()+1) : 0)
  , m_compressMethod(     compressMethod                          )
{
}

NTindexNode *NTindexNode::allocateNTindexNode(UINT NTindex, const Grammar &grammar, const StatePairArray &statePairArray) {
  const Options              &options = Options::getInstance();
  const NTindexNodeCommonData commonData(NTindex, grammar);
  if(!options.m_useTableCompression) {
    return new BinSearchNode(nullptr, commonData, statePairArray);
  } else {
    return allocateNode(nullptr, MixedSuccessorTable(commonData, statePairArray));
  }
}

NTindexNode *NTindexNode::allocateNode(const NTindexNode *parent, const MixedSuccessorTable &mst) {
  const UINT     newStateCountArray  = mst.m_statePairArray.getNewStateCount();
  const UINT     newStateCountBitSet = mst.m_statePairBitSetArray.getNewStateCount();
  const UINT     newStateCount       = newStateCountArray + newStateCountBitSet;
  const Options &options             = Options::getInstance();

  assert(newStateCount >= 1);

  if(newStateCount == 1) {
    if(Options::getInstance().m_pruneSuccTransBitSet) {
      return allocateImmediateDontCareNode(parent, mst);
    }
    if(newStateCountArray == 1) {
      return allocateStatePairArrayNode(parent, mst, mst.m_statePairArray);
    }
  }

  const BYTE recurseLevel = parent ? parent->getRecurseLevel() + 1 : 0;
  if((newStateCountBitSet == 0) || (mst.m_statePairBitSetArray.first().getFromStateCount() < options.m_minStateBitSetSize) || (recurseLevel >= options.m_maxRecursionTransSucc)) {
    return allocateStatePairArrayNode(parent, mst, mst.mergeAll());
  }
  return allocateSplitNode(parent, mst);
}

// assume (fromStateCount + statePairSetCount >= 1);
NTindexNode *NTindexNode::allocateImmediateDontCareNode(const NTindexNode *parent, const MixedSuccessorTable &mst) {
  const UINT   newStateCountArray  = mst.m_statePairArray.getNewStateCount();
  const UINT   newStateCountBitSet = mst.m_statePairBitSetArray.getNewStateCount();
  const UINT   newStateCount       = newStateCountArray + newStateCountBitSet;
  assert(newStateCount == 1);
  const UINT   newState            = (newStateCountArray == 1) ? mst.m_statePairArray.first().m_newState : mst.m_statePairBitSetArray.first().getNewState();
  NTindexNode *p                   = new ImmediateNode(parent, mst, newState, mst.getFromStateCount()); TRACE_NEW(p);
  return p;
}

NTindexNode *NTindexNode::allocateSplitNode(const NTindexNode *parent, const MixedSuccessorTable &mst) {
  const UINT   fromStateCount      = mst.getFromStateCount();
  SplitNode   *p                   = new SplitNode(parent, mst, fromStateCount); TRACE_NEW(p);
  // (newStateCount >= 2) && (newStateCountBitSet >= 1) && (m_statePairBitSetArray.first().getFromStateCount() >= options.m_minStateBitSetSize)
  NTindexNode *child0 = allocateStatePairBitSetNode(p, mst, mst.m_statePairBitSetArray[0]);
  NTindexNode *child1 = allocateNode(               p, MixedSuccessorTable(mst).removeFirstStatePairBitSet());
  p->setChild(0, child0).setChild(1, child1);
  return p;
}

NTindexNode *NTindexNode::allocateStatePairArrayNode(const NTindexNode *parent, const NTindexNodeCommonData &cd, const StatePairArray &statePairArray) {
  NTindexNode *p;
  if(statePairArray.getFromStateCount() == 1) {
    p = new ImmediateNode(parent, cd, statePairArray[0]); TRACE_NEW(p);
  } else {
    p = new BinSearchNode(parent, cd, statePairArray   ); TRACE_NEW(p);
  }
  return p;
}

NTindexNode *NTindexNode::allocateStatePairBitSetNode(const NTindexNode *parent, const NTindexNodeCommonData &cd, const StatePairBitSet &statePairBitSet) {
  NTindexNode *p;
  if(statePairBitSet.getFromStateCount() == 1) {
    const StatePairArray spa(statePairBitSet);
    p = new ImmediateNode(parent, cd, spa[0]      ); TRACE_NEW(p);
  } else {
    p = new BitSetNode(   parent, cd, statePairBitSet); TRACE_NEW(p);
  }
  return p;
}

SplitNode::~SplitNode() {
  SAFEDELETE(m_child[0]);
  SAFEDELETE(m_child[1]);
}

SplitNode &SplitNode::setChild(BYTE index, NTindexNode *child) {
  assert(index < 2);
  assert(m_child[index] == nullptr);
  m_child[index] = child;
  return *this;
}

String NTindexNode::toString() const {
  return format(_T("%u - NTindex %u %s %-20s (From states:%u)\n")
               ,m_recurseLevel
               ,m_NTindex
               ,getSymbolName().cstr()
               ,compressMethodToString(getCompressionMethod())
               ,getFromStateCount()
               );
}

String BinSearchNode::toString() const {
  const String result = __super::toString()  + indentString(m_statePairArray.toString(),3);
  return indentString(result, m_recurseLevel * 2);
}

String SplitNode::toString() const {
  const String cstr0  = indentString(getChild(0).toString(),2);
  const String cstr1  = indentString(getChild(1).toString(),2);
  const String result = __super::toString() + indentString(format(_T("Child 0:\n%s\nChild 1:\n%s"), cstr0.cstr(), cstr1.cstr()), 3);
  return indentString(result, m_recurseLevel * 2);
}

String ImmediateNode::toString() const {
  const String result = __super::toString() + indentString(m_statePair.toString(),3);
  return indentString(result, m_recurseLevel * 2);
}

String BitSetNode::toString() const {
  const String result = __super::toString() + indentString(m_statePairBitSet.toString(),3);
  return indentString(result, m_recurseLevel * 2);
}

}; // namespace TransSuccMatrixCompression
