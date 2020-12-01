#pragma once

#include "GrammarTables.h"
#include "CompressEncoding.h"
#include "StatePair.h"
#include "SymbolNodeBase.h"

class MixedStatePairArray : public SymbolNodeBase {
public:
  StatePairArray              m_statePairArray;
  StatePairBitSetArray        m_statePairBitSetArray;
  MixedStatePairArray(const SymbolNodeBase &base, const StatePairArray &statePairArray);
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

class SymbolNode : public SymbolNodeBase {
private:
  SymbolNode(           const SymbolNode &src); // not implemented
  SymbolNode &operator=(const SymbolNode &src); // not implemented
protected:
  const SymbolNode         *m_parent;
  const UINT               m_fromStateCount;
  const BYTE               m_recurseLevel;
  const CompressionMethod  m_compressMethod;
  SymbolNode(const SymbolNode *parent, const SymbolNodeBase &base, UINT fromStateCount, CompressionMethod compressMethod);
  static SymbolNode *allocateNode(                 const SymbolNode *parent, const MixedStatePairArray &msp);
  static SymbolNode *allocateSplitNode(            const SymbolNode *parent, const MixedStatePairArray &msp);
  static SymbolNode *allocateStatePairArrayNode(   const SymbolNode *parent, const SymbolNodeBase &base , const StatePairArray  &statePairArray );
  static SymbolNode *allocateStatePairBitSetNode(  const SymbolNode *parent, const SymbolNodeBase &base , const StatePairBitSet &statePairBitSet);
  // assume (fromStateCount + statePairSetCount >= 1 && newStateCount == 1);
  static SymbolNode *allocateImmediateDontCareNode(const SymbolNode *parent, const MixedStatePairArray &msp);

public:
  static SymbolNode *allocateSymbolNode(const Grammar &grammar, UINT symbol, const StatePairArray &statePairArray, const MatrixOptimizeParameters &opt);
  virtual          ~SymbolNode() {
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
    return *new StatePairArray(); // should never come here
  }
  // Call only if getCompressionMethod() == CompCodeSplitNode
  virtual const SymbolNode           &getChild(BYTE index)   const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *this; // should never come here
  }
  // Call only if getCompressionMethod() == CompCodeImmediate
  virtual const StatePair            &getStatePair()         const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *new StatePair(); // should never come here
  }
  // Call only if getCompressionMethod() == CompCodeImmediate
  virtual bool                        isDontCareNode()       const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return false; // should never come here
  }
  // Call only if getCompressionMethod() == CompCodeBitSet
  virtual const StatePairBitSet      &getStatePairBitSet()   const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *new StatePairBitSet(getGrammar(), 0,0, isTerminal()); // should never come here
  }
  virtual String                      toString()             const;
};

class SymbolNodeBinSearch : public SymbolNode {
private:
  StatePairArray m_statePairArray;
public:
  SymbolNodeBinSearch(const SymbolNode *parent, const SymbolNodeBase &base, const StatePairArray &statePairArray)
    : SymbolNode(parent, base, statePairArray.getFromStateCount(), CompCodeBinSearch)
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

class SymbolNodeSplit : public SymbolNode {
private:
  const SymbolNode *m_child[2];
public:
  SymbolNodeSplit(const SymbolNode *parent, const SymbolNodeBase &base, UINT fromStateCount)
    : SymbolNode(parent, base, fromStateCount, CompCodeSplitNode)
  {
    m_child[0] = m_child[1] = nullptr;
  }
  ~SymbolNodeSplit() override;
  SymbolNodeSplit &setChild(BYTE index, SymbolNode *child);
  const SymbolNode &getChild(BYTE index) const override {
    assert(index < 2);
    return *m_child[index];
  }
  String toString() const override;
};

class SymbolNodeImmediate : public SymbolNode {
private:
  const StatePair m_statePair;
public:
  SymbolNodeImmediate(const SymbolNode *parent, const SymbolNodeBase &base, const StatePair &statePair)
    : SymbolNode(parent, base, 1, CompCodeImmediate)
    , m_statePair(statePair)
  {
  }
  SymbolNodeImmediate(const SymbolNode *parent, const SymbolNodeBase &base, UINT newState, UINT fromStateCount)
    : SymbolNode(parent, base, fromStateCount, CompCodeImmediate)
    , m_statePair(StatePair(StatePair::NoFromStateCheck, newState, false))
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

class SymbolNodeBitSet : public SymbolNode {
private:
  const StatePairBitSet m_statePairBitSet;
public:
  SymbolNodeBitSet(const SymbolNode *parent, const SymbolNodeBase &base, const StatePairBitSet &statePairBitSet)
    : SymbolNode(parent, base, statePairBitSet.getFromStateCount(), CompCodeBitSet)
    , m_statePairBitSet(statePairBitSet)
  {
  }
  const StatePairBitSet &getStatePairBitSet() const override {
    return m_statePairBitSet;
  }
  String toString() const override;
};
