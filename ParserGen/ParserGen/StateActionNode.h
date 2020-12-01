#pragma once

#include "Grammar.h"
#include "CompressEncoding.h"
#include "TermSetReduction.h"

namespace ActionMatrixCompression {

class ActionNodeCommonData {
private:
  const Grammar                 &m_grammar;
  const UINT                     m_state;
  const MatrixOptimizeParameters m_opt;
public:
  ActionNodeCommonData(const Grammar &grammar, UINT state, const MatrixOptimizeParameters &opt)
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

class ShiftAndReduceActions : public ActionNodeCommonData {
public:
  ParserActionArray          m_shiftActionArray;
  TermSetReductionArray      m_termSetReductionArray;
  ShiftAndReduceActions(const ActionNodeCommonData &cd, const ParserActionArray &actionArray);
  ParserActionArray mergeAll() const;
  inline UINT getLegalTermCount() const {
    return m_shiftActionArray.getLegalTermCount() + m_termSetReductionArray.getLegalTermCount();
  }
  ShiftAndReduceActions &removeFirstTermSet() {
    m_termSetReductionArray.removeIndex(0);
    return *this;
  }
};

class StateActionNode : public ActionNodeCommonData {
private:
  StateActionNode(           const StateActionNode &src); // not implemented
  StateActionNode &operator=(const StateActionNode &src); // not implemented
protected:
  const StateActionNode     *m_parent;
  const UINT                 m_legalTermCount;
  const BYTE                 m_recurseLevel;
  const CompressionMethod    m_compressMethod;
  StateActionNode(const StateActionNode *parent, const ActionNodeCommonData &cd, UINT legalTermCount, CompressionMethod compressMethod)
    : ActionNodeCommonData(cd)
    , m_parent(         parent         )
    , m_legalTermCount( legalTermCount )
    , m_recurseLevel(   parent?(parent->getRecurseLevel()+1) : 0)
    , m_compressMethod( compressMethod )
  {
  }
  static StateActionNode          *allocateNode(           const StateActionNode *parent, const ShiftAndReduceActions &sra);
  static StateActionNode          *allocateBinSearchNode(  const StateActionNode *parent, const ActionNodeCommonData  &cd , const ParserActionArray &actionArray     );
  static StateActionNode          *allocateSplitNode(      const StateActionNode *parent, const ShiftAndReduceActions &sra);
  static StateActionNode          *allocateBitSetNode(     const StateActionNode *parent, const ActionNodeCommonData  &cd , const TermSetReduction  &termSetReduction);
public:
  static StateActionNode          *allocateStateActionNode(const Grammar &grammar, UINT state, const ParserActionArray &actionArray, const MatrixOptimizeParameters &opt);
  virtual                         ~StateActionNode() {
  }

  inline UINT                      getLegalTermCount()    const {
    return m_legalTermCount;
  }
  inline BYTE                      getRecurseLevel()      const {
    return m_recurseLevel;
  }
  inline CompressionMethod         getCompressionMethod() const {
    return m_compressMethod;
  }
  // Call only if getCompressionMethod() == CompCodeBinSearch
  virtual const ParserActionArray &getTermList()          const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *new ParserActionArray();
  }
  // Call only if getCompressionMethod() == CompCodeSplitNode
  virtual const StateActionNode   &getChild(BYTE index)   const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *this;
  }
  // Call only if getCompressionMethod() == CompCodeImmediate
  virtual ParserAction             getOneItemAction()     const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return ParserAction();
  }
  // Call only if getCompressionMethod() == CompCodeBitSet
  virtual const TermSetReduction  &getTermSetReduction()  const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *new TermSetReduction(getGrammar(), 0,0);
  }
  virtual String toString() const;
};

class BinSearchNode : public StateActionNode {
private:
  ParserActionArray  m_termListActionArray;
public:
  BinSearchNode(const StateActionNode *parent, const ActionNodeCommonData &cd, const ParserActionArray &termListActionArray)
    : StateActionNode(parent, cd, termListActionArray.getLegalTermCount(), CompCodeBinSearch)
    , m_termListActionArray(termListActionArray)
  {
  }
  const ParserActionArray &getTermList() const override {
    return m_termListActionArray;
  }
  String toString() const override;
};

class SplitNode : public StateActionNode {
private:
  const StateActionNode *m_child[2];
public:
  SplitNode(const StateActionNode *parent, const ActionNodeCommonData &cd, UINT legalTermCount)
    : StateActionNode(parent, cd, legalTermCount, CompCodeSplitNode)
  {
    m_child[0] = m_child[1] = nullptr;
  }
  ~SplitNode() override;
  SplitNode &setChild(BYTE index, StateActionNode *child);
  const StateActionNode &getChild(BYTE index) const override {
    assert(index < 2);
    return *m_child[index];
  }
  String toString() const override;
};

class ImmediateNode : public StateActionNode {
private:
  const ParserAction m_action;
public:
  ImmediateNode(const StateActionNode *parent, const ActionNodeCommonData &cd, ParserAction action)
    : StateActionNode(parent, cd, 1, CompCodeImmediate)
    , m_action(action)
  {
  }
  ParserAction getOneItemAction() const override {
    return m_action;
  }
  String toString() const override;
};

class BitSetNode : public StateActionNode {
private:
  const TermSetReduction m_termSetReduction;
public:
  BitSetNode(const StateActionNode *parent, const ActionNodeCommonData &cd, const TermSetReduction &termSetReduction)
    : StateActionNode(parent, cd, termSetReduction.getLegalTermCount(), CompCodeBitSet)
    , m_termSetReduction(termSetReduction)
  {
  }
  const TermSetReduction &getTermSetReduction() const override {
    return m_termSetReduction;
  }
  String toString() const override;
};

}; // namespace ActionMatrixCompression
