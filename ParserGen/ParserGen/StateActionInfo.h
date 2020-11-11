#pragma once

#include "GrammarTables.h"
#include "CompressEncoding.h"

class TermSetReduction {
private:
  const UINT                 m_prod;
  UINT                       m_termSetSize; // == m_termSet.size()
  SymbolSet                  m_termSet;     // set of terminals which should give reduce by production m_prod
  const SymbolNameContainer &m_symbolNames;
public:
  TermSetReduction(UINT terminalCount, UINT prod, UINT term0, const SymbolNameContainer &symbolNames)
    : m_prod(       prod         )
    , m_symbolNames(symbolNames  )
    , m_termSetSize(0            )
    , m_termSet(    terminalCount)
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
    m_termSetSize++;
  }
  inline UINT getTermSetSize() const {
    return m_termSetSize;
  }
  operator ActionArray() const;
  String toString() const {
    return format(_T("Reduce by %u on %s (%u terminals)"), m_prod, m_symbolNames.symbolSetToString(m_termSet).cstr(), m_termSetSize);
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
  UINT getLegalTokenCount() const {
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

class StateActionInfo {
private:
  const StateActionInfo     *m_parent;
  const UINT                 m_state, m_terminalCount, m_legalTokenCount;
  BYTE                       m_recurseLevel;
  const SymbolNameContainer &m_symbolNames;
  CompressionMethod          m_compressMethod;
  ActionArray               *m_termListActionArray;      // use this for m_compressMethod = {ParserTables::CompCodeTermList, ParserTables::CompCodeOneItem}
  TermSetReduction          *m_termSetReduction; //      == null, unless m_compressMethod == ParserTables::CompCodeTermSet
  const StateActionInfo     *m_child[2];         // both == null, unless m_compressMethod == ParserTables::CompCodeSplitNode
  // merge all actions from the 2 arrays together in 1 sorted ActionArray (compressMethod = CompCodeTermList
  StateActionInfo(           const StateActionInfo &src); // not implemented
  StateActionInfo &operator=(const StateActionInfo &src); // not implemented
  StateActionInfo(  const StateActionInfo *parent, const TermSetReduction     &termSetReduction   );
  StateActionInfo(  const StateActionInfo *parent, const ActionArray          &termListActionArray);
  StateActionInfo(  const StateActionInfo *parent, const ActionArray          &shiftActionArray, const TermSetReductionArray &termSetReductionArray);
  void initPointers(const StateActionInfo *parent);
  CompressionMethod  setTermListCompression(       const ActionArray          &termListActionArray);
  CompressionMethod  setTermSetReduceCompression(  const TermSetReduction     &termSetReduction   );
  CompressionMethod  findCompressionMethod(        const ActionArray          &shiftActionArray, const TermSetReductionArray &termSetReductionArray);
  static ActionArray mergeActionArrays(            const ActionArray          &shiftActionArray, const TermSetReductionArray &termSetReductionArray);
public:
  StateActionInfo(UINT terminalCount, UINT state,  const ActionArray          &actionArray     , const SymbolNameContainer   &symbolNames);
  ~StateActionInfo();
  inline UINT                        getState()             const {
    return m_state;
  }
  inline UINT                        getLegalTokenCount()   const {
    return m_legalTokenCount;
  }
  inline CompressionMethod           getCompressionMethod() const {
    return m_compressMethod;
  }
  // Call only if getCompressionMethod() == CompCodeTermList
  inline const ActionArray          &getTermList()          const {
    assert(getCompressionMethod() == ParserTables::CompCodeTermList);
    return *m_termListActionArray;
  }
  // Call only if getCompressionMethod() == CompCodeOneItem
  inline ParserAction                getOneItemAction()     const {
    assert(getCompressionMethod() == ParserTables::CompCodeOneItem);
    return (*m_termListActionArray)[0];
  }
  // Call only if getCompressionMethod() == ParserTables::CompCodeTermSet
  inline const TermSetReduction     &getTermSetReduction()  const {
    assert(getCompressionMethod() == ParserTables::CompCodeTermSet);
    return *m_termSetReduction;
  }
  // Call only if getCompressionMethod() == ParserTables::CompCodeSplitNode
  const StateActionInfo             &getChild(BYTE index)   const {
    assert(getCompressionMethod() == ParserTables::CompCodeSplitNode);
    assert(index < 2);
    return *m_child[index];
  }

  String toString() const;
};
