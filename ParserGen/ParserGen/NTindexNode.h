#pragma once

#include "GrammarTables.h"
#include "CompressEncoding.h"

namespace TransposedSuccessorMatrixCompression {

class StatePair {
public:
  static constexpr UINT NoFromStateCheck = AbstractParserTables::_NoFromStateCheck;
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

inline std::wostream &operator<<(std::wostream &out, const StatePair &p) {
  out << p.toString();
  return out;
}

inline int statePairCompareFromState(const StatePair &p1, const StatePair &p2) {
  return (int)p1.m_fromState - (int)p2.m_fromState;
}

inline int statePairCmpByNewState(const StatePair &p1, const StatePair &p2) {
  int c = (int)p1.m_newState - (int)p2.m_newState;
  if(c) return c;
  return (int)p1.m_fromState - (int)p2.m_fromState;
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

class NTIndexNodeCommonData {
public:
  const UINT     m_ntIndex;
  const Grammar &m_grammar;
  NTIndexNodeCommonData(UINT ntIndex, const Grammar &grammar) : m_ntIndex(ntIndex), m_grammar(grammar) {
  }
  inline UINT getNTIndex() const {
    return m_ntIndex;
  }
  inline const String &getSymbolName() const {
    return m_grammar.getSymbolName(m_grammar.getTermCount() + m_ntIndex);
  }
};

class MixedStatePairArray : public NTIndexNodeCommonData {
public:
  StatePairArray              m_statePairArray;
  StatePairBitSetArray        m_statePairBitSetArray;
  MixedStatePairArray(const NTIndexNodeCommonData &cd, const StatePairArray &statePairArray);
  StatePairArray mergeAll() const;
  inline UINT    getFromStateCount() const {
    return m_statePairArray.getFromStateCount() + m_statePairBitSetArray.getFromStateCount();
  }
  inline UINT    getNewStateCount() const {
    return m_statePairArray.getNewStateCount() + m_statePairBitSetArray.getNewStateCount();
  }
  MixedStatePairArray &removeFirstBitSet() {
    m_statePairBitSetArray.removeIndex(0);
    return *this;
  }
};

class NTIndexNode : public NTIndexNodeCommonData {
private:
  NTIndexNode(           const NTIndexNode &src); // not implemented
  NTIndexNode &operator=(const NTIndexNode &src); // not implemented
protected:
  const NTIndexNode          *m_parent;
  const UINT                  m_fromStateCount;
  const BYTE                  m_recurseLevel;
  const CompressionMethod     m_compressMethod;
  NTIndexNode(const NTIndexNode *parent, const NTIndexNodeCommonData &cd, UINT fromStateCount, CompressionMethod compressMethod);
  static NTIndexNode *allocateNode(                 const NTIndexNode *parent, const MixedStatePairArray   &msp);
  static NTIndexNode *allocateSplitNode(            const NTIndexNode *parent, const MixedStatePairArray   &msp);
  static NTIndexNode *allocateStatePairArrayNode(   const NTIndexNode *parent, const NTIndexNodeCommonData &cd , const StatePairArray  &statePairArray );
  static NTIndexNode *allocateStatePairBitSetNode(  const NTIndexNode *parent, const NTIndexNodeCommonData &cd , const StatePairBitSet &statePairBitSet);
  static NTIndexNode *allocateImmediateDontCareNode(const NTIndexNode *parent, const MixedStatePairArray   &msp);
public:
  static NTIndexNode *allocateNTIndexNode(UINT ntIndex, const Grammar &grammar, const StatePairArray &statePairArray);
  virtual            ~NTIndexNode() {
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
  virtual const NTIndexNode          &getChild(BYTE index)   const {
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

class BinSearchNode : public NTIndexNode {
private:
  StatePairArray m_statePairArray;
public:
  BinSearchNode(const NTIndexNode *parent, const NTIndexNodeCommonData &cd, const StatePairArray &statePairArray)
    : NTIndexNode(parent, cd, statePairArray.getFromStateCount(), CompCodeBinSearch)
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

class SplitNode : public NTIndexNode {
private:
  const NTIndexNode *m_child[2];
public:
  SplitNode(const NTIndexNode *parent, const NTIndexNodeCommonData &cd, UINT fromStateCount)
    : NTIndexNode(parent, cd, fromStateCount, CompCodeSplitNode)
  {
    m_child[0] = m_child[1] = nullptr;
  }
  ~SplitNode() override;
  SplitNode &setChild(BYTE index, NTIndexNode *child);
  const NTIndexNode &getChild(BYTE index) const override {
    assert(index < 2);
    return *m_child[index];
  }
  String toString() const override;
};

class ImmediateNode : public NTIndexNode {
private:
  const StatePair m_statePair;
public:
  ImmediateNode(const NTIndexNode *parent, const NTIndexNodeCommonData &cd, const StatePair &statePair)
    : NTIndexNode(parent, cd, 1, CompCodeImmediate)
    , m_statePair(statePair)
  {
  }
  ImmediateNode(const NTIndexNode *parent, const NTIndexNodeCommonData &cd, UINT newState, UINT fromStateCount)
    : NTIndexNode(parent, cd, fromStateCount, CompCodeImmediate)
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

class BitSetNode : public NTIndexNode {
private:
  const StatePairBitSet m_statePairBitSet;
public:
  BitSetNode(const NTIndexNode *parent, const NTIndexNodeCommonData &cd, const StatePairBitSet &statePairBitSet)
    : NTIndexNode(parent, cd, statePairBitSet.getFromStateCount(), CompCodeBitSet)
    , m_statePairBitSet(statePairBitSet)
  {
  }
  const StatePairBitSet &getStatePairBitSet() const override {
    return m_statePairBitSet;
  }
  String toString() const override;
};

}; // namespace TransposedSuccessorMatrixCompression
