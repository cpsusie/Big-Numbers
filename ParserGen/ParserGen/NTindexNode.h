#pragma once

#include "CompressEncoding.h"

namespace TransSuccMatrixCompression {

class StatePair {
public:
  UINT m_fromState;
  UINT m_newState;
  inline StatePair() : m_fromState(0), m_newState(0) {
  }
  inline StatePair(UINT fromState, UINT newState) : m_fromState(fromState), m_newState(newState) {
  }
  String toString() const {
    return format(_T("Goto %u from %u"), m_newState, m_fromState);
  }
};

inline int statePairCompareFromState(const StatePair &p1, const StatePair &p2) {
  return (int)p1.m_fromState - (int)p2.m_fromState;
}

inline int statePairCmpByNewState(const StatePair &p1, const StatePair &p2) {
  return (int)p1.m_newState - (int)p2.m_newState;
}

class StatePairArray : public CompactArray<StatePair> {
public:
  inline StatePairArray() {
  }
  inline StatePairArray(UINT capacity) : CompactArray(capacity) {
  }
  inline StatePairArray &sortByFromState() {
    sort(statePairCompareFromState);
    return *this;
  }
  inline StatePairArray &sortByNewState() {
    sort(statePairCmpByNewState);
    return *this;
  }
  StateSet       getFromStateSet(UINT stateCount) const;
  StateArray     getNewStateArray()               const;
  inline UINT    getFromStateCount()              const {
    return(UINT)size();
  }
  String         toString() const;
};

class StatePairSet {
private:
  const UINT m_newState;
  StateSet   m_fromStateSet;     // set of states having newState as successor
  UINT       m_fromStateCount;
public:
  StatePairSet(UINT newState, UINT fromState0, UINT stateCount)
    : m_newState(      newState   )
    , m_fromStateSet(  stateCount )
    , m_fromStateCount(0          )
  {
    addFromState(fromState0);
  }
  inline UINT getNewState() const {
    return m_newState;
  }
  inline const StateSet &getFromStateSet() const {
    return m_fromStateSet;
  }
  inline void addFromState(UINT fromState) {
    m_fromStateSet.add(fromState);
    m_fromStateCount++;
  }
  inline UINT getFromStateCount() const {
    return m_fromStateCount;
  }
  operator StatePairArray() const;
  String toString() const {
    return format(_T("Goto %u from %s (%u states)"), m_newState, m_fromStateSet.toRangeString().cstr(), getFromStateCount());
  }
};

inline int setSizeReverseCmp(const StatePairSet &sp1, const StatePairSet &sp2) {
  return (int)sp2.getFromStateCount() - (int)sp1.getFromStateCount();
}

class StatePairSetArray : public Array<StatePairSet> {
public:
  StatePairSetArray() {
  }
  StatePairSetArray(size_t capacity) : Array(capacity) {
  }
  // sort by setSize, decreasing, ie. largest set first
  inline void sortBySetSize() {
    if(size() > 1) {
      sort(setSizeReverseCmp);
    }
  }
  // Return sum(((*this)[i].getFromSetSize()...i=0..size-1)
  UINT getFromStateCount() const {
    UINT sum = 0;
    for(auto it = getIterator(); it.hasNext();) {
      sum += it.next().getFromStateCount();
    }
    return sum;
  }
  String toString() const {
    String result;
    for(auto it = getIterator(); it.hasNext();) {
      result += format(_T("   %s\n"), it.next().toString().cstr());
    }
    return result;
  }
};

class NTindexNodeCommonData {
public:
  const UINT                  m_NTindex;
  const AbstractParserTables &m_tables;
  NTindexNodeCommonData(UINT NTindex, const AbstractParserTables &tables) : m_NTindex(NTindex), m_tables(tables) {
  }
  inline UINT getNTindex() const {
    return m_NTindex;
  }
};

class MixedSuccessorTable : public NTindexNodeCommonData {
public:
  StatePairArray              m_statePairArray;
  StatePairSetArray           m_statePairSetArray;
  MixedSuccessorTable(const NTindexNodeCommonData &cd, const StatePairArray &statePairArray);
  StatePairArray mergeAll() const;
  inline UINT    getFromStateCount() const {
    return m_statePairArray.getFromStateCount() + m_statePairSetArray.getFromStateCount();
  }
  MixedSuccessorTable &removeFirstStatePairSet() {
    m_statePairSetArray.removeIndex(0);
    return *this;
  }
};

class NTindexNode : public NTindexNodeCommonData {
private:
  NTindexNode(           const NTindexNode &src); // not implemented
  NTindexNode &operator=(const NTindexNode &src); // not implemented
protected:
  const NTindexNode          *m_parent;
  const UINT                  m_fromStateCount;
  const BYTE                  m_recurseLevel;
  const CompressionMethod     m_compressMethod;
  NTindexNode(const NTindexNode *parent, const NTindexNodeCommonData &cd, UINT fromStateCount, CompressionMethod compressMethod);
  static NTindexNode *allocateNode(             const NTindexNode *parent, const MixedSuccessorTable  &mst);
  static NTindexNode *allocateSplitNode(        const NTindexNode *parent, const MixedSuccessorTable  &mst);
  static NTindexNode *allocateStatePairListNode(const NTindexNode *parent, const NTindexNodeCommonData &cd , const StatePairArray &statePairArray);
  static NTindexNode *allocateStatePairSetNode( const NTindexNode *parent, const NTindexNodeCommonData &cd , const StatePairSet   &statePairSet  );
public:
  static NTindexNode *allocateNTindexNode(UINT NTindex, const AbstractParserTables &tables, const StatePairArray &statePairArray);
  virtual            ~NTindexNode() {
  }

  inline UINT                         getFromStateCount()    const {
    return m_fromStateCount;
  }
  inline BYTE                         getRecurseLevel()      const {
    return m_recurseLevel;
  }
  inline CompressionMethod            getCompressionMethod() const {
    return m_compressMethod;
  }
  // Call only if getCompressionMethod() == CompCodeTermList
  virtual const StatePairArray       &getStatePairList()     const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *new StatePairArray();
  }
  // Call only if getCompressionMethod() == CompCodeOneItem
  virtual const StatePair            &getStatePair()         const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *new StatePair();
  }
  // Call only if getCompressionMethod() == ParserTables::CompCodeTermSet
  virtual const StatePairSet         &getStatePairSet()      const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *new StatePairSet(0,0,0);
  }
  // Call only if getCompressionMethod() == ParserTables::CompCodeSplitNode
  virtual const NTindexNode          &getChild(BYTE index)   const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *this;
  }
  virtual String toString() const;
};

class StatePairListNode : public NTindexNode {
private:
  StatePairArray m_statePairArray;
public:
  StatePairListNode(const NTindexNode *parent, const NTindexNodeCommonData &cd, const StatePairArray &statePairArray)
    : NTindexNode(parent, cd, statePairArray.getFromStateCount(), AbstractParserTables::CompCodeBinSearch)
    , m_statePairArray(statePairArray)
  {
    m_statePairArray.sortByFromState();
  }
  const StatePairArray &getStatePairList() const override {
    return m_statePairArray;
  }
  String toString() const override;
};

class SplitNode : public NTindexNode {
private:
  const NTindexNode *m_child[2];
public:
  SplitNode(const NTindexNode *parent, const NTindexNodeCommonData &cd, UINT fromStateCount)
    : NTindexNode(parent, cd, fromStateCount, AbstractParserTables::CompCodeSplitNode)
  {
    m_child[0] = m_child[1] = nullptr;
  }
  ~SplitNode() override;
  SplitNode &setChild(BYTE index, NTindexNode *child);
  const NTindexNode &getChild(BYTE index) const override {
    assert(index < 2);
    return *m_child[index];
  }
  String toString() const override;
};

class OneStatePairNode : public NTindexNode {
private:
  const StatePair m_statePair;
public:
  OneStatePairNode(const NTindexNode *parent, const NTindexNodeCommonData &cd, const StatePair &statePair)
    : NTindexNode(parent, cd, 1, AbstractParserTables::CompCodeImmediate)
    , m_statePair(statePair)
  {
  }
  const StatePair &getStatePair() const override {
    return m_statePair;
  }
  String toString() const override;
};

class StatePairSetNode : public NTindexNode {
private:
  const StatePairSet m_statePairSet;
public:
  StatePairSetNode(const NTindexNode *parent, const NTindexNodeCommonData &cd, const StatePairSet &statePairSet)
    : NTindexNode(parent, cd, statePairSet.getFromStateCount(), AbstractParserTables::CompCodeBitset)
    , m_statePairSet(statePairSet)
  {
  }
  const StatePairSet &getStatePairSet() const override {
    return m_statePairSet;
  }
  String toString() const override;
};

}; // namespace TransSuccMatrixCompression
