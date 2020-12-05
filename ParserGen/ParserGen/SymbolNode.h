#pragma once

#include "CompressEncoding.h"
#include "SymbolNodeBase.h"

class MixedStatePairArray;
class StatePair;
class StatePairArray;
class StatePairBitSet;

class SymbolNode : public SymbolNodeBase {
private:
  SymbolNode(           const SymbolNode &src); // not implemented
  SymbolNode &operator=(const SymbolNode &src); // not implemented
protected:
  const SymbolNode        *m_parent;
  const UINT               m_fromStateCount;
  const BYTE               m_recurseLevel;
  const CompressionMethod  m_compressMethod;
  SymbolNode(const SymbolNode *parent, const SymbolNodeBase &base, UINT fromStateCount, CompressionMethod compressMethod)
    : SymbolNodeBase(  base                                    )
    , m_parent(        parent                                  )
    , m_fromStateCount(fromStateCount                          )
    , m_recurseLevel(  parent?(parent->getRecurseLevel()+1) : 0)
    , m_compressMethod(compressMethod                          )
  {
  }

  static SymbolNode *allocateNode(                 const SymbolNode *parent, const MixedStatePairArray &ma);
  static SymbolNode *allocateSplitNode(            const SymbolNode *parent, const MixedStatePairArray &ma);
  static SymbolNode *allocateStatePairArrayNode(   const SymbolNode *parent, const SymbolNodeBase      &base , const StatePairArray  &statePairArray );
  static SymbolNode *allocateStatePairBitSetNode(  const SymbolNode *parent, const SymbolNodeBase      &base , const StatePairBitSet &statePairBitSet);
  // assume (fromStateCount + statePairSetCount >= 1 && newStateCount == 1);
  static SymbolNode *allocateImmediateDontCareNode(const SymbolNode *parent, const MixedStatePairArray &ma);

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
  virtual const StatePairArray &getStatePairArray()    const;
  // Call only if getCompressionMethod() == CompCodeSplitNode
  virtual const SymbolNode           &getChild(BYTE index)   const;
  // Call only if getCompressionMethod() == CompCodeImmediate
  virtual const StatePair            &getStatePair()         const;
  // Call only if getCompressionMethod() == CompCodeImmediate
  virtual bool                        isDontCareNode()       const;
  // Call only if getCompressionMethod() == CompCodeBitSet
  virtual const StatePairBitSet      &getStatePairBitSet()   const;
  virtual String                      toString()             const;
};
