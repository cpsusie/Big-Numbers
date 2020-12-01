#pragma once

#include "Grammar.h"
#include "CompressEncoding.h"
#include "TermSetReduction.h"

namespace TransposedShiftMatrixCompression {

class ReduceNodeCommonData {
private:
  const Grammar                 &m_grammar;
  const UINT                     m_state;
  const MatrixOptimizeParameters m_opt;
public:
  ReduceNodeCommonData(const Grammar &grammar, UINT state, const MatrixOptimizeParameters &opt)
   : m_grammar(grammar)
   , m_state(  state  )
   , m_opt(    opt    )
  {
  }
  const Grammar &getGrammar() const {
    return m_grammar;
  }
  inline UINT getState() const {
    return m_state;
  }
  inline const MatrixOptimizeParameters &getOptimizeParam() const {
    return m_opt;
  }
};

class MixedReductionArray : public ReduceNodeCommonData {
public:
  ParserActionArray     m_parserActionArray;
  TermSetReductionArray m_termSetReductionArray;
  MixedReductionArray(const ReduceNodeCommonData &cd, const ParserActionArray &actionArray);
  ParserActionArray mergeAll() const;
  inline UINT       getLegalTermCount() const {
    return m_parserActionArray.getLegalTermCount() + m_termSetReductionArray.getLegalTermCount();
  }
  MixedReductionArray  &removeFirstTermSet() {
    m_termSetReductionArray.removeIndex(0);
    return *this;
  }
};

class ReduceNode : public ReduceNodeCommonData {
private:
  ReduceNode(           const ReduceNode &src); // not implemented
  ReduceNode &operator=(const ReduceNode &src); // not implemented
protected:
  const ReduceNode       *m_parent;
  const UINT              m_legalTermCount;
  const BYTE              m_recurseLevel;
  const CompressionMethod m_compressMethod;
  ReduceNode(const ReduceNode *parent, const ReduceNodeCommonData &cd, UINT legalTermCount, CompressionMethod compressMethod)
    : ReduceNodeCommonData(cd)
    , m_parent(         parent         )
    , m_legalTermCount( legalTermCount )
    , m_recurseLevel(   parent?(parent->getRecurseLevel()+1) : 0)
    , m_compressMethod( compressMethod )
  {
  }
  static ReduceNode          *allocateNode(           const ReduceNode *parent, const MixedReductionArray &sra);
  static ReduceNode          *allocateBinSearchNode(  const ReduceNode *parent, const ReduceNodeCommonData  &cd , const ParserActionArray &actionArray     );
  static ReduceNode          *allocateSplitNode(      const ReduceNode *parent, const MixedReductionArray &sra);
  static ReduceNode          *allocateBitSetNode(     const ReduceNode *parent, const ReduceNodeCommonData  &cd , const TermSetReduction  &termSetReduction);
public:
  static ReduceNode          *allocateReduceNode(const Grammar &grammar, UINT state, const ParserActionArray &actionArray, const MatrixOptimizeParameters &opt);
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
  virtual const ParserActionArray &getParserActionArray() const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *new ParserActionArray();
  }
  // Call only if getCompressionMethod() == CompCodeSplitNode
  virtual const ReduceNode        &getChild(BYTE index)   const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *this;
  }
  // Call only if getCompressionMethod() == CompCodeImmediate
  virtual const ParserAction      &getParserAction()     const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *new ParserAction();
  }
  // Call only if getCompressionMethod() == CompCodeBitSet
  virtual const TermSetReduction  &getTermSetReduction()  const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *new TermSetReduction(getGrammar(), 0,0);
  }
  virtual String toString() const;
};

class ReduceNodeBinSearch : public ReduceNode {
private:
  const ParserActionArray m_parserActionArray;
public:
  ReduceNodeBinSearch(const ReduceNode *parent, const ReduceNodeCommonData &cd, const ParserActionArray &parserActionArray)
    : ReduceNode(parent, cd, parserActionArray.getLegalTermCount(), CompCodeBinSearch)
    , m_parserActionArray(parserActionArray)
  {
  }
  const ParserActionArray &getParserActionArray() const override {
    return m_parserActionArray;
  }
  String toString() const override;
};

class ReduceNodeSplit : public ReduceNode {
private:
  const ReduceNode *m_child[2];
public:
  ReduceNodeSplit(const ReduceNode *parent, const ReduceNodeCommonData &cd, UINT legalTermCount)
    : ReduceNode(parent, cd, legalTermCount, CompCodeSplitNode)
  {
    m_child[0] = m_child[1] = nullptr;
  }
  ~ReduceNodeSplit() override;
  ReduceNodeSplit &setChild(BYTE index, ReduceNode *child);
  const ReduceNode &getChild(BYTE index) const override {
    assert(index < 2);
    return *m_child[index];
  }
  String toString() const override;
};

class ReduceNodeImmediate : public ReduceNode {
private:
  const ParserAction m_action;
public:
  ReduceNodeImmediate(const ReduceNode *parent, const ReduceNodeCommonData &cd, ParserAction action)
    : ReduceNode(parent, cd, 1, CompCodeImmediate)
    , m_action(action)
  {
  }
  const ParserAction &getParserAction() const override {
    return m_action;
  }
  String toString() const override;
};

class ReduceNodeBitSet : public ReduceNode {
private:
  const TermSetReduction m_termSetReduction;
public:
  ReduceNodeBitSet(const ReduceNode *parent, const ReduceNodeCommonData &cd, const TermSetReduction &termSetReduction)
    : ReduceNode(parent, cd, termSetReduction.getLegalTermCount(), CompCodeBitSet)
    , m_termSetReduction(termSetReduction)
  {
  }
  const TermSetReduction &getTermSetReduction() const override {
    return m_termSetReduction;
  }
  String toString() const override;
};

}; // namespace TransposedShiftMatrixCompression
