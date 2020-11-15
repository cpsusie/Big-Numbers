#pragma once

#include "GrammarTables.h"
#include "CompressEncoding.h"

class TermSetReduction {
private:
  const UINT                 m_prod;
  const SymbolNameContainer &m_nameContainer;
  SymbolSet                  m_termSet;     // set of terminals which should give reduce by production m_prod
  UINT                       m_setSize;
public:
  TermSetReduction(UINT prod, UINT term0, const SymbolNameContainer &nameContainer)
    : m_prod(         prod                            )
    , m_nameContainer(nameContainer                   )
    , m_termSet(      nameContainer.getTerminalCount())
    , m_setSize(      0                               )
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
  inline UINT getTermSetSize() const {
    return m_setSize;
  }
  operator ParserActionArray() const;
  String toString() const {
    return format(_T("Reduce by %u on %s (%u terminals)"), m_prod, m_nameContainer.symbolSetToString(m_termSet).cstr(), m_setSize);
  }
};

inline int setSizeReverseCmp(const TermSetReduction &i1, const TermSetReduction &i2) {
  return (int)i2.getTermSetSize() - (int)i1.getTermSetSize();
}

class TermSetReductionArray : public Array<TermSetReduction> {
public:
  TermSetReductionArray() {
  }
  TermSetReductionArray(size_t capacity) : Array<TermSetReduction>(capacity) {
  }
  // sort by setSize, decreasing, ie. largest set first
  inline void sortBySetSize() {
    if(size() > 1) {
      sort(setSizeReverseCmp);
    }
  }
  // Return sum(((*this)[i].getTermSetSize()...i=0..size-1)
  UINT getLegalTermCount() const {
    UINT sum = 0;
    for(auto it = getIterator(); it.hasNext();) {
      sum += it.next().getTermSetSize();
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

class InfoNodeCommonData {
public:
  const UINT                 m_state;
  const SymbolNameContainer &m_nameContainer;
  InfoNodeCommonData(UINT state, const SymbolNameContainer &nameContainer) : m_state(state), m_nameContainer(nameContainer) {
  }
};

class ShiftAndReduceActions : public InfoNodeCommonData {
public:
  ParserActionArray          m_shiftActionArray;
  TermSetReductionArray      m_termSetReductionArray;
  ShiftAndReduceActions(const InfoNodeCommonData &cd, const ParserActionArray &actionArray);
  ParserActionArray mergeAll() const;
  inline UINT getLegalTermCount() const {
    return m_shiftActionArray.getLegalTermCount() + m_termSetReductionArray.getLegalTermCount();
  }
  ShiftAndReduceActions &removeFirstTermSet() {
    m_termSetReductionArray.removeIndex(0);
    return *this;
  }
};

class StateActionInfo : public InfoNodeCommonData {
private:
  StateActionInfo(           const StateActionInfo &src); // not implemented
  StateActionInfo &operator=(const StateActionInfo &src); // not implemented
protected:
  const StateActionInfo     *m_parent;
  const UINT                 m_legalTermCount;
  const BYTE                 m_recurseLevel;
  const CompressionMethod    m_compressMethod;
  StateActionInfo(const StateActionInfo *parent, const InfoNodeCommonData &cd, UINT legalTermCount, CompressionMethod compressMethod)
    : InfoNodeCommonData(cd)
    , m_parent(         parent         )
    , m_legalTermCount( legalTermCount )
    , m_recurseLevel(   parent?(parent->getRecurseLevel()+1) : 0)
    , m_compressMethod( compressMethod )
  {
  }
  static StateActionInfo *allocateNode(        const StateActionInfo *parent, const ShiftAndReduceActions &sra);
  static StateActionInfo *allocateSplitNode(   const StateActionInfo *parent, const ShiftAndReduceActions &sra);
  static StateActionInfo *allocateTermListNode(const StateActionInfo *parent, const InfoNodeCommonData    &cd , const ParserActionArray &actionArray     );
  static StateActionInfo *allocateTermSetNode( const StateActionInfo *parent, const InfoNodeCommonData    &cd , const TermSetReduction  &termSetReduction);
public:
  static StateActionInfo             *allocateStateActionInfo(UINT state, const SymbolNameContainer &nameContainer, const ParserActionArray &actionArray);
  virtual                            ~StateActionInfo() {
  }

  inline UINT                         getState()             const {
    return m_state;
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
  // Call only if getCompressionMethod() == CompCodeTermList
  virtual const ParserActionArray    &getTermList()          const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *new ParserActionArray();
  }
  // Call only if getCompressionMethod() == CompCodeOneItem
  virtual ParserAction                getOneItemAction()     const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return ParserAction();
  }
  // Call only if getCompressionMethod() == ParserTables::CompCodeTermSet
  virtual const TermSetReduction     &getTermSetReduction()  const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *new TermSetReduction(0,0,m_nameContainer);
  }
  // Call only if getCompressionMethod() == ParserTables::CompCodeSplitNode
  virtual const StateActionInfo      &getChild(BYTE index)   const {
    throwUnsupportedOperationException(__TFUNCTION__);
    __assume(0);
    return *this;
  }
  virtual String toString() const;
};

class TermListNode : public StateActionInfo {
private:
  ParserActionArray  m_termListActionArray;
public:
  TermListNode(const StateActionInfo *parent, const InfoNodeCommonData &cd, const ParserActionArray &termListActionArray)
    : StateActionInfo(parent, cd, termListActionArray.getLegalTermCount(), ParserTables::CompCodeTermList)
    , m_termListActionArray(termListActionArray)
  {
  }
  const ParserActionArray &getTermList() const override {
    return m_termListActionArray;
  }
  String toString() const override;
};

class SplitNode : public StateActionInfo {
private:
  const StateActionInfo *m_child[2];
public:
  SplitNode(const StateActionInfo *parent, const InfoNodeCommonData &cd, UINT legalTermCount)
    : StateActionInfo(parent, cd, legalTermCount, ParserTables::CompCodeSplitNode)
  {
    m_child[0] = m_child[1] = nullptr;
  }
  ~SplitNode() override;
  SplitNode &setChild(BYTE index, StateActionInfo *child);
  const StateActionInfo &getChild(BYTE index) const override {
    assert(index < 2);
    return *m_child[index];
  }
  String toString() const override;
};

class OneItemNode : public StateActionInfo {
private:
  const ParserAction m_action;
public:
  OneItemNode(const StateActionInfo *parent, const InfoNodeCommonData &cd, ParserAction action)
    : StateActionInfo(parent, cd, 1, ParserTables::CompCodeOneItem)
    , m_action(action)
  {
  }
  ParserAction getOneItemAction() const override {
    return m_action;
  }
  String toString() const override;
};

class TermSetNode : public StateActionInfo {
private:
  const TermSetReduction m_termSetReduction;
public:
  TermSetNode(const StateActionInfo *parent, const InfoNodeCommonData &cd, const TermSetReduction &termSetReduction)
    : StateActionInfo(parent, cd, termSetReduction.getTermSetSize(), ParserTables::CompCodeTermSet)
    , m_termSetReduction(termSetReduction)
  {
  }
  const TermSetReduction &getTermSetReduction() const override {
    return m_termSetReduction;
  }
  String toString() const override;
};
