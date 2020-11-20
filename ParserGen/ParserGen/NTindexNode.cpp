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

StatePairSet::operator StatePairArray() const {
  StatePairArray result(getFromStateCount());
  for(auto it = m_fromStateSet.getIterator(); it.hasNext();) {
    result.add(StatePair((USHORT)it.next(), m_newState));
  }
  return result;
}

MixedSuccessorTable::MixedSuccessorTable(const NTindexNodeCommonData &cd, const StatePairArray &statePairArray)
: NTindexNodeCommonData(cd)
{
  CompactUIntHashMap<UINT, 256>  sameNewStateMap(241); // map from newState -> index into m_statePairSetArray
  for(const StatePair sp : statePairArray) {
    const UINT  newState = sp.m_newState;
    const UINT *indexp   = sameNewStateMap.get(newState);
    if(indexp) {
      m_statePairSetArray[*indexp].addFromState(sp.m_fromState);
    } else {
      const UINT index = (UINT)m_statePairSetArray.size();
      m_statePairSetArray.add(StatePairSet(newState, sp.m_fromState, m_tables.getStateCount()));
      sameNewStateMap.put(newState, index);
    }
  }
  m_statePairSetArray.sortBySetSize();
}

StatePairArray MixedSuccessorTable::mergeAll() const {
  StatePairArray result(getFromStateCount());
  result.addAll(m_statePairArray);
  if(!m_statePairSetArray.isEmpty()) {
    for(auto it = m_statePairSetArray.getIterator(); it.hasNext();) {
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

NTindexNode *NTindexNode::allocateNTindexNode(UINT NTindex, const AbstractParserTables &tables, const StatePairArray &statePairArray) {
  const NTindexNodeCommonData commonData(NTindex, tables);
  const Options             &options = Options::getInstance();
  if(!options.m_useTableCompression) {
    return new StatePairListNode(nullptr, commonData, statePairArray);
  } else {
    return allocateNode(nullptr, MixedSuccessorTable(commonData, statePairArray));
  }
}

NTindexNode *NTindexNode::allocateNode(const NTindexNode *parent, const MixedSuccessorTable &mst) {
  const Options &options           = Options::getInstance();
  const UINT     fromStateCount    = mst.m_statePairArray.getFromStateCount();
  const UINT     statePairSetCount = (UINT)mst.m_statePairSetArray.size();

  assert(fromStateCount + statePairSetCount >= 1);

  if(fromStateCount + statePairSetCount == 1) {
    if(statePairSetCount == 0) {
      return allocateStatePairListNode(parent, mst, mst.m_statePairArray);
    } else { // reduceActions == 1
      return allocateStatePairSetNode( parent, mst, mst.m_statePairSetArray[0]);
    }
  }

  const BYTE recurseLevel = parent ? parent->getRecurseLevel() + 1 : 0;
  if((statePairSetCount == 0) || (mst.m_statePairSetArray[0].getFromStateCount() < 2) || (recurseLevel >= options.m_maxRecursiveCalls)) {
    return allocateStatePairListNode(parent, mst, mst.mergeAll());
  }
  return allocateSplitNode(parent, mst);
}

NTindexNode *NTindexNode::allocateSplitNode(const NTindexNode *parent, const MixedSuccessorTable &mst) {
  const UINT fromStateCount = mst.getFromStateCount();
  SplitNode *p = new SplitNode(parent, mst, fromStateCount); TRACE_NEW(p);
  // (shiftActions + reduceActions >= 2) && (reduceActions >= 1) && (m_termSetReductionArray[0].getTermSetSize() >= 2)
  NTindexNode *child0 = allocateStatePairSetNode(p, mst, mst.m_statePairSetArray[0]);
  NTindexNode *child1 = allocateNode(            p, MixedSuccessorTable(mst).removeFirstStatePairSet());
  p->setChild(0, child0).setChild(1, child1);
  return p;
}

NTindexNode *NTindexNode::allocateStatePairListNode(const NTindexNode *parent, const NTindexNodeCommonData &cd, const StatePairArray &statePairArray) {
  NTindexNode *p;
  if(statePairArray.size() == 1) {
    p = new OneStatePairNode( parent, cd, statePairArray[0]); TRACE_NEW(p);
  } else {
    p = new StatePairListNode(parent, cd, statePairArray   ); TRACE_NEW(p);
  }
  return p;
}


NTindexNode *NTindexNode::allocateStatePairSetNode(const NTindexNode *parent, const NTindexNodeCommonData &cd, const StatePairSet &statePairSet) {
  NTindexNode *p;
  if(statePairSet.getFromStateCount() == 1) {
    const StatePairArray spa(statePairSet);
    p = new OneStatePairNode(parent, cd, spa[0]      ); TRACE_NEW(p);
  } else {
    p = new StatePairSetNode(parent, cd, statePairSet); TRACE_NEW(p);
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
  return format(_T("NTindex %u %-20s recurseLevel:%u, (From states:%u)\n"), m_NTindex, compressMethodToString(getCompressionMethod()), m_recurseLevel, getFromStateCount());
}

String StatePairListNode::toString() const {
  const String result = __super::toString()  + indentString(m_statePairArray.toString(),3);
  return indentString(result, m_recurseLevel * 2);
}

String SplitNode::toString() const {
  const String cstr0  = indentString(getChild(0).toString(),2);
  const String cstr1  = indentString(getChild(1).toString(),2);
  const String result = __super::toString() + indentString(format(_T("Child 0:\n%s\nChild 1:\n%s"), cstr0.cstr(), cstr1.cstr()), 3);
  return indentString(result, m_recurseLevel * 2);
}

String OneStatePairNode::toString() const {
  const String result = __super::toString() + indentString(m_statePair.toString(),3);
  return indentString(result, m_recurseLevel * 2);
}

String StatePairSetNode::toString() const {
  const String result = __super::toString() + indentString(m_statePairSet.toString(),3);
  return indentString(result, m_recurseLevel * 2);
}

}; // namespace TransSuccMatrixCompression
