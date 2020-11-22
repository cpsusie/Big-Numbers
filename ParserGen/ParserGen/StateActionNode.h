#pragma once

#include "GrammarTables.h"
#include "CompressEncoding.h"

namespace ActionMatrixCompression {

class TermSetReduction {
private:
  const UINT                         m_prod;
  const AbstractSymbolNameContainer &m_nameContainer;
  SymbolSet                          m_termSet;     // set of terminals which should give reduce by production m_prod
  UINT                               m_setSize;
public:
  TermSetReduction(UINT prod, UINT term0, const AbstractSymbolNameContainer &nameContainer)
    : m_prod(         prod                        )
    , m_nameContainer(nameContainer               )
    , m_termSet(      nameContainer.getTermCount())
    , m_setSize(      0                           )
  {
    addTerminal(term0);
  }
  inline UINT getProduction() const {
    return m_prod;
  }
  inline const SymbolSet &getTermSet() const {
    return m_termSet;
  }
  inline void addTerminal(UINT term) {
    m_termSet.add(term);
    m_setSize++;
  }
  inline UINT getLegalTermCount() const {
    return m_setSize;
  }
  operator ParserActionArray() const;
  String toString() const {
    return format(_T("Reduce by %u on %s (%u terminals)"), m_prod, m_nameContainer.symbolSetToString(m_termSet).cstr(), m_setSize);
  }
};

inline int legalTermCountReverseCmp(const TermSetReduction &i1, const TermSetReduction &i2) {
  return (int)i2.getLegalTermCount() - (int)i1.getLegalTermCount();
}

class TermSetReductionArray : public Array<TermSetReduction> {
public:
  TermSetReductionArray() {
  }
  TermSetReductionArray(size_t capacity) : Array(capacity) {
  }
  // sort by setSize, decreasing, ie. largest set first
  inline void sortByLegalTermCount() {
    if(size() > 1) {
      sort(legalTermCountReverseCmp);
    }
  }
  // Return sum(((*this)[i].getTermSetSize()...i=0..size-1)
  UINT getLegalTermCount() const {
    UINT sum = 0;
    for(auto it = getIterator(); it.hasNext();) {
      sum += it.next().getLegalTermCount();
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

class ActionNodeCommonData {
public:
  const UINT                         m_state;
  const AbstractSymbolNameContainer &m_nameContainer;
  ActionNodeCommonData(UINT state, const AbstractSymbolNameContainer &nameContainer)
   : m_state(state)
   , m_nameContainer(nameContainer)
  {
  }
  inline UINT                getState() const {
    return m_state;
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
  static StateActionNode *allocateNode(         const StateActionNode *parent, const ShiftAndReduceActions &sra);
  static StateActionNode *allocateBinSearchNode(const StateActionNode *parent, const ActionNodeCommonData    &cd , const ParserActionArray &actionArray     );
  static StateActionNode *allocateSplitNode(    const StateActionNode *parent, const ShiftAndReduceActions &sra);
  static StateActionNode *allocateBitSetNode(   const StateActionNode *parent, const ActionNodeCommonData    &cd , const TermSetReduction  &termSetReduction);
public:
  static StateActionNode             *allocateStateActionNode(UINT state, const AbstractSymbolNameContainer &nameContainer, const ParserActionArray &actionArray);
  virtual                            ~StateActionNode() {
  }

  inline UINT                         getLegalTermCount()    const {
    return m_legalTermCount;
  }
  inline BYTE                         getRecurseLevel()      const {
    return m_recurseLevel;
  }
  inline CompressionMethod            getCompressionMethod() const {
    return m_compressMethod;
  }
  // Call only if getCompressionMethod() == CompCodeBinSearch
  virtual const ParserActionArray    &getTermList()          const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *new ParserActionArray();
  }
  // Call only if getCompressionMethod() == CompCodeSplitNode
  virtual const StateActionNode      &getChild(BYTE index)   const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *this;
  }
  // Call only if getCompressionMethod() == CompCodeImmediate
  virtual ParserAction                getOneItemAction()     const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return ParserAction();
  }
  // Call only if getCompressionMethod() == CompCodeBitSet
  virtual const TermSetReduction     &getTermSetReduction()  const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *new TermSetReduction(0,0,m_nameContainer);
  }
  virtual String toString() const;
};

class BinSearchNode : public StateActionNode {
private:
  ParserActionArray  m_termListActionArray;
public:
  BinSearchNode(const StateActionNode *parent, const ActionNodeCommonData &cd, const ParserActionArray &termListActionArray)
    : StateActionNode(parent, cd, termListActionArray.getLegalTermCount(), AbstractParserTables::CompCodeBinSearch)
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
    : StateActionNode(parent, cd, legalTermCount, AbstractParserTables::CompCodeSplitNode)
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
    : StateActionNode(parent, cd, 1, AbstractParserTables::CompCodeImmediate)
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
    : StateActionNode(parent, cd, termSetReduction.getLegalTermCount(), AbstractParserTables::CompCodeBitSet)
    , m_termSetReduction(termSetReduction)
  {
  }
  const TermSetReduction &getTermSetReduction() const override {
    return m_termSetReduction;
  }
  String toString() const override;
};

}; // namespace ActionMatrixCompression
