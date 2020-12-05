#pragma once

#include "CompressEncoding.h"
#include "ReduceNodeBase.h"

class MixedReductionArray;
class TermActionPair;
class TermActionPairArray;
class TermSetReduction;

class ReduceNode : public ReduceNodeBase {
private:
  ReduceNode(           const ReduceNode &src); // not implemented
  ReduceNode &operator=(const ReduceNode &src); // not implemented
protected:
  const ReduceNode       *m_parent;
  const UINT              m_legalTermCount;
  const BYTE              m_recurseLevel;
  const CompressionMethod m_compressMethod;
  ReduceNode(const ReduceNode *parent, const ReduceNodeBase &base, UINT legalTermCount, CompressionMethod compressMethod)
    : ReduceNodeBase(   base           )
    , m_parent(         parent         )
    , m_legalTermCount( legalTermCount )
    , m_recurseLevel(   parent?(parent->getRecurseLevel()+1) : 0)
    , m_compressMethod( compressMethod )
  {
  }
  static ReduceNode          *allocateNode(           const ReduceNode *parent, const MixedReductionArray &ma);
  static ReduceNode          *allocateSplitNode(      const ReduceNode *parent, const MixedReductionArray &ma);
  static ReduceNode          *allocateBinSearchNode(  const ReduceNode *parent, const ReduceNodeBase      &base, const TermActionPairArray &termActionArray );
  static ReduceNode          *allocateBitSetNode(     const ReduceNode *parent, const ReduceNodeBase      &base, const TermSetReduction    &termSetReduction);
public:
  static ReduceNode          *allocateReduceNode(const Grammar &grammar, UINT state, const TermActionPairArray &termActionArray, const MatrixOptimizeParameters &opt);
  virtual                    ~ReduceNode() {
  }

  inline UINT                 getLegalTermCount()    const {
    return m_legalTermCount;
  }
  inline BYTE                 getRecurseLevel()      const {
    return m_recurseLevel;
  }
  inline CompressionMethod    getCompressionMethod() const {
    return m_compressMethod;
  }
  // Call only if getCompressionMethod() == CompCodeBinSearch
  virtual const TermActionPairArray &getTermActionPairArray() const;
  // Call only if getCompressionMethod() == CompCodeSplitNode
  virtual const ReduceNode          &getChild(BYTE index)     const;
  // Call only if getCompressionMethod() == CompCodeImmediate
  virtual const TermActionPair      &getTermActionPair()      const;
  // Call only if getCompressionMethod() == CompCodeBitSet
  virtual const TermSetReduction    &getTermSetReduction()    const;

  virtual String toString() const;
};
