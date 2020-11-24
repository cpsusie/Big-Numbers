#pragma once

#include "GrammarTables.h"
#include "CompressEncoding.h"

namespace TransSuccMatrixCompression {

class StatePair {
public:
  static constexpr UINT NoFromStateCheck = 0x7fff; // special value for fromState, indicating always goto newState, no matter which state we come from
  UINT m_fromState;
  UINT m_newState;
  inline StatePair() : m_fromState(0), m_newState(0) {
  }
  inline StatePair(UINT fromState, UINT newState) : m_fromState(fromState), m_newState(newState) {
    assert(m_newState != NoFromStateCheck);
  }
  inline bool isNoFromStateCheck() const {
    return m_fromState == NoFromStateCheck;
  }
  String toString() const {
    return isNoFromStateCheck()
         ? format(_T("Goto %u"        ), m_newState)
         : format(_T("Goto %u from %u"), m_newState, m_fromState);
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
  // Return all legal from-states in array as a BitSet
  StateSet       getFromStateSet(UINT stateCount) const;
  // Return all (no neccessary distinct) newstates in array
  StateArray     getNewStateArray()               const;
  // = size() = number of different from-states in array
  inline UINT    getFromStateCount()              const {
    return(UINT)size();
  }
  // = size() = number of different to-states in array
  inline UINT    getNewStateCount()               const {
    return (UINT)size();
  }
  String         toString() const;
};

class StatePairBitSet {
private:
  const Grammar &m_grammar;
  StateSet       m_fromStateSet;     // set of states having newState as successor
  UINT           m_fromStateCount;
  const UINT     m_newState;
public:
  StatePairBitSet(UINT newState, UINT fromState0, const Grammar &grammar)
    : m_grammar(         grammar                )
    , m_fromStateSet(    grammar.getStateCount())
    , m_fromStateCount(  0                      )
    , m_newState(        newState               )
  {
    addFromState(fromState0);
  }
  inline const StateSet &getFromStateSet()   const {
    return m_fromStateSet;
  }
  inline UINT            getFromStateCount() const {
    return m_fromStateCount;
  }
  // = 1
  inline UINT            getNewStateCount()  const {
    return 1;
  }
  inline UINT            getNewState()       const {
    return m_newState;
  }
  inline void            addFromState(UINT fromState) {
    m_fromStateSet.add(fromState);
    m_fromStateCount++;
  }
  operator               StatePairArray()    const;
  String                 toString()          const {
    return format(_T("Goto %u from %s (%u states)"), m_newState, m_fromStateSet.toRangeString().cstr(), getFromStateCount());
  }
};

inline int setSizeReverseCmp(const StatePairBitSet &sp1, const StatePairBitSet &sp2) {
  return (int)sp2.getFromStateCount() - (int)sp1.getFromStateCount();
}

class StatePairBitSetArray : public Array<StatePairBitSet> {
public:
  StatePairBitSetArray() {
  }
  StatePairBitSetArray(size_t capacity) : Array(capacity) {
  }
  // sort by setSize, decreasing, ie. largest set first
  inline void sortBySetSize() {
    if(size() > 1) {
      sort(setSizeReverseCmp);
    }
  }
  // Return sum(((*this)[i].getFromStateCount()...i=0..size-1)
  UINT getFromStateCount() const {
    UINT sum = 0;
    for(auto it = getIterator(); it.hasNext();) {
      sum += it.next().getFromStateCount();
    }
    return sum;
  }
  // = size() = number of different to-states in array
  inline UINT    getNewStateCount() const {
    return (UINT)size();
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
  const UINT     m_NTindex;
  const Grammar &m_grammar;
  NTindexNodeCommonData(UINT NTindex, const Grammar &grammar) : m_NTindex(NTindex), m_grammar(grammar) {
  }
  inline UINT getNTindex() const {
    return m_NTindex;
  }
  inline const String &getSymbolName() const {
    return m_grammar.getSymbolName(m_grammar.getTermCount() + m_NTindex);
  }
};

class MixedSuccessorTable : public NTindexNodeCommonData {
public:
  StatePairArray              m_statePairArray;
  StatePairBitSetArray        m_statePairBitSetArray;
  MixedSuccessorTable(const NTindexNodeCommonData &cd, const StatePairArray &statePairArray);
  StatePairArray mergeAll() const;
  inline UINT    getFromStateCount() const {
    return m_statePairArray.getFromStateCount() + m_statePairBitSetArray.getFromStateCount();
  }
  inline UINT    getNewStateCount() const {
    return m_statePairArray.getNewStateCount() + m_statePairBitSetArray.getNewStateCount();
  }
  MixedSuccessorTable &removeFirstStatePairBitSet() {
    m_statePairBitSetArray.removeIndex(0);
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
  static NTindexNode *allocateNode(                 const NTindexNode *parent, const MixedSuccessorTable  &mst);
  static NTindexNode *allocateSplitNode(            const NTindexNode *parent, const MixedSuccessorTable  &mst);
  static NTindexNode *allocateStatePairArrayNode(   const NTindexNode *parent, const NTindexNodeCommonData &cd , const StatePairArray  &statePairArray );
  static NTindexNode *allocateStatePairBitSetNode(  const NTindexNode *parent, const NTindexNodeCommonData &cd , const StatePairBitSet &statePairBitSet);
  static NTindexNode *allocateImmediateDontCareNode(const NTindexNode *parent, const MixedSuccessorTable  &mst);
public:
  static NTindexNode *allocateNTindexNode(UINT NTindex, const Grammar &grammar, const StatePairArray &statePairArray);
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
  // Call only if getCompressionMethod() == CompCodeBinSearch
  virtual const StatePairArray       &getStatePairArray()    const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *new StatePairArray();
  }
  // Call only if getCompressionMethod() == CompCodeSplitNode
  virtual const NTindexNode          &getChild(BYTE index)   const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *this;
  }
  // Call only if getCompressionMethod() == CompCodeImmediate
  virtual const StatePair            &getStatePair()         const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *new StatePair();
  }
  // Call only if getCompressionMethod() == CompCodeImmediate
  virtual bool                        isDontCareNode()       const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return false;
  }
  // Call only if getCompressionMethod() == CompCodeBitSet
  virtual const StatePairBitSet         &getStatePairBitSet()      const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *new StatePairBitSet(0,0,m_grammar);
  }
  virtual String toString() const;
};

class BinSearchNode : public NTindexNode {
private:
  StatePairArray m_statePairArray;
public:
  BinSearchNode(const NTindexNode *parent, const NTindexNodeCommonData &cd, const StatePairArray &statePairArray)
    : NTindexNode(parent, cd, statePairArray.getFromStateCount(), AbstractParserTables::CompCodeBinSearch)
    , m_statePairArray(statePairArray)
  {
    assert(statePairArray.getFromStateCount() >= 2);
    m_statePairArray.sortByFromState();
  }
  const StatePairArray &getStatePairArray() const override {
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

class ImmediateNode : public NTindexNode {
private:
  const StatePair m_statePair;
public:
  ImmediateNode(const NTindexNode *parent, const NTindexNodeCommonData &cd, const StatePair &statePair)
    : NTindexNode(parent, cd, 1, AbstractParserTables::CompCodeImmediate)
    , m_statePair(statePair)
  {
  }
  ImmediateNode(const NTindexNode *parent, const NTindexNodeCommonData &cd, UINT newState, UINT fromStateCount)
    : NTindexNode(parent, cd, fromStateCount, AbstractParserTables::CompCodeImmediate)
    , m_statePair(StatePair(StatePair::NoFromStateCheck, newState))
  {
  }
  bool isDontCareNode() const override {
    return m_statePair.isNoFromStateCheck();
  }
  const StatePair &getStatePair() const override {
    return m_statePair;
  }
  String toString() const override;
};

class BitSetNode : public NTindexNode {
private:
  const StatePairBitSet m_statePairBitSet;
public:
  BitSetNode(const NTindexNode *parent, const NTindexNodeCommonData &cd, const StatePairBitSet &statePairBitSet)
    : NTindexNode(parent, cd, statePairBitSet.getFromStateCount(), AbstractParserTables::CompCodeBitSet)
    , m_statePairBitSet(statePairBitSet)
  {
  }
  const StatePairBitSet &getStatePairBitSet() const override {
    return m_statePairBitSet;
  }
  String toString() const override;
};

}; // namespace TransSuccMatrixCompression
